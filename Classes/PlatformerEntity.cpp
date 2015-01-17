#include <map>
#include <functional>
#include <limits>
#include "PlatformerEntity.h"
#include "helpers.h"
#include "PlatformerLayer.h"

#define CRASHONNULL \
	if (Game == nullptr)\
		{\
		log("Game is null!");\
		unscheduleAllSelectors(); \
		return; \
		}

#define CRASHONNULL2(ret) \
if (Game == nullptr)\
{\
	log("Game is null!"); \
	unscheduleAllSelectors(); \
	return (ret); \
}

namespace Platformer
{
	const float FADEDIETIME = 0.3f;
	const int TAG_PLAYER_PARTICLE = 10000;
	const float BIG_NUMBER = numeric_limits<float>::max();
	PlatformerLayer* Entity::Game = nullptr;

	Entity::Entity()
	{
		position = Position{ 0, 0 };
		player = false;
		harmful = false;
		pushable = false;
		pusher = false;
		dead = false;
		collect = false;
		timeBombActive = false;
		immortal = false;
		_r = 255;
		_g = 255;
		_b = 255;
		initZ = 0;
	}

	bool Entity::touch(Entity* it, b2Fixture* myFixture, b2Fixture* itsFixture)
	{
		if (myFixture->GetUserData() == FIX_BASE &&
			itsFixture->GetUserData() == FIX_BASE)
			return touch(it);
		return false;
	}

	b2Fixture* Entity::createShape(b2FixtureDef* blockShapeDef)
	{
		return createBoxShape(blockShapeDef);
	}

	b2Fixture* Entity::createBoxShape(b2FixtureDef* blockShapeDef)
	{
		b2PolygonShape blockShape;
		blockShape.SetAsBox(
			0.95f * getContentSize().width / PTM_RATIO,
			0.95f * getContentSize().height / PTM_RATIO);

		blockShapeDef->shape = &blockShape;
		return body->CreateFixture(blockShapeDef);
	}

	b2Fixture* Entity::createCircleShape(b2FixtureDef* blockShapeDef)
	{
		b2CircleShape blockShape;
		blockShape.m_radius =
			0.95f * getContentSize().width / PTM_RATIO;

		blockShapeDef->shape = &blockShape;
		return body->CreateFixture(blockShapeDef);
	}

	void Entity::syncBodyPosition()
	{
		body->SetTransform(b2Vec2(
			(getPositionX()) / PTM_RATIO,
			(getPositionY()) / PTM_RATIO), body->GetAngle());
	}

	void Entity::createPhysics()
	{
		auto aabb = getBoundingBox();

		b2BodyDef blockBodyDef;
		blockBodyDef.type = b2_staticBody;
		blockBodyDef.position.Set(
			(getPositionX() - PlatformerLayer::blockSize * 0.5f) / PTM_RATIO,
			(getPositionY() - PlatformerLayer::blockSize * 0.5f) / PTM_RATIO);
		blockBodyDef.userData = this;
		body = Game->world->CreateBody(&blockBodyDef);
		body->SetLinearDamping(0.5f);

		// Create block shape
		b2FixtureDef blockShapeDef;

		blockShapeDef.density = 1.0;
		blockShapeDef.friction = 0.0f;
		blockShapeDef.restitution = 0.6f;
		blockShapeDef.userData = FIX_BASE;
		auto f = createShape(&blockShapeDef); //body->CreateFixture(&blockShapeDef);
		f->SetUserData(FIX_BASE);
		body->SetUserData(this);
	}

	void Entity::sendMessage(const string& msg)
	{
		CRASHONNULL;
		Game->broadcast(this, msg);
	}

	Sprite* Entity::AddSprite(const vector<string>& _spriteNames)
	{
		return AddSprite(_spriteNames[rand() % _spriteNames.size()]);
	}

	Sprite* Entity::AddSprite(const string& _spriteName)
	{
		setContentSize(Size(0.5f + PlatformerLayer::blockSize * 0.5f, 0.5f + PlatformerLayer::blockSize * 0.5f));
		auto sprite = MAKESPRITE(_spriteName);
		auto spriteSize = sprite->getContentSize();
		sprite->setScale(PlatformerLayer::blockSize / (spriteSize.width - 0.5f));
		addChild(sprite);
		sprite->setTag(0);
		return sprite;
	}

	void Entity::activateTimeBomb(float duration)
	{
		if (timeBombActive) return;
		timeBombActive = true;
		scheduleOnce(schedule_selector(Entity::explode), duration);
	}

	bool Entity::move(Directions d)
	{
		CRASHONNULL2(false);
		return Game->move(this, d);
	}

	void Entity::setDefaultColor(int r, int g, int b)
	{
		_r = r;
		_g = g;
		_b = b;
		if (sprite) sprite->setColor(Color3B(r, g, b));
	}

	void Entity::die()
	{
		CRASHONNULL;
		dead = true;
		harmful = false;
		sendMessage(EMSG_DIED);
		sprite->runAction(FadeOut::create(FADEDIETIME));
		sprite->runAction(ScaleTo::create(FADEDIETIME, 0.01f));
		scheduleOnce(schedule_selector(Entity::doCollect), FADEDIETIME);

		b2Filter f;
		f.maskBits = 0x0000;
		for (auto fix = body->GetFixtureList(); fix; fix = fix->GetNext())
			fix->SetFilterData(f);
	}

	void Entity::doCollect(float)
	{
		CRASHONNULL;
		stopAllActions();
		removeAllChildren();
		unscheduleAllSelectors();
	}

	void Entity::fadeDie(float delay)
	{
		sprite->runAction(FadeOut::create(delay));
		activateTimeBomb(delay);
	}

	void Entity::gravityChanged(bool flipped)
	{
		body->SetTransform(body->GetPosition(), body->GetAngle() + b2_pi);
	}

	void Entity::shine(int r, int g, int b, float timeIn, float timeOut)
	{
		Sequence* seq = Sequence::create(
			TintTo::create(timeIn, r, g, b),
			TintTo::create(timeOut, _r, _g, _b),
			NULL);
		sprite->runAction(seq);
	}

	void Entity::stopMoving()
	{
		if (collect) return;
		unschedule(schedule_selector(Entity::moveAction));
		setPosition(PlatformerLayer::blockSize * position.x, PlatformerLayer::blockSize * position.y);
		moveDelta = Point::ZERO;
		moveTimeLeft = 0;
	}

	void Entity::moveAction(float dt)
	{
		if (collect) return;
		moveTimeLeft -= dt;
		if (moveTimeLeft <= 0 || moveDelta == Point::ZERO || collect)
		{
			stopMoving();
		}
		auto currentPos = getPosition();
		currentPos += moveDelta * dt;
		setPosition(currentPos);
	}

	void Entity::moveTo(Point p, float t)
	{
		if (collect) return;
		stopMoving();
		if (t <= 0) return;
		if (p == getPosition()) return;
		moveDest = p;
		moveTimeLeft = t;
		moveDelta = (p - getPosition()) / t;
		schedule(schedule_selector(Entity::moveAction));
	}

	void Fake::createPhysics()
	{
		Base::createPhysics();
		b2Filter f;
		f.maskBits = 0x0000;
		for (auto fix = body->GetFixtureList(); fix; fix = fix->GetNext())
			fix->SetFilterData(f);
	}

	Wall::Wall()
	{
		tintOnTouch = true;
		_playerContacts = 0;
		_soundEnabled = true;
		immortal = true;
	}

	bool Wall::touch(Entity* e)
	{
		return false;
	}

	bool Wall::touch(Entity* e, b2Fixture* me, b2Fixture* it)
	{
		if (e->player && tintOnTouch)
		{
			float r = fmodf(e->getPositionX(), Game->blockSize * 7.0f) / (Game->blockSize * 7.0f);
			auto color = hsl2Rgb(r, 1.0f, 0.7f);
			shine(color.r, color.g, color.b);

			if (_soundEnabled)
			{
				unschedule(schedule_selector(Wall::enableSound));
				_soundEnabled = false;
				PlayEffect("piano.wav", r);
			}

			_playerContacts++;
		}

		return Base::touch(e, me, it);
	}

	void Wall::touchEnded(Entity* e, b2Fixture* me, b2Fixture* it)
	{
		if (e->player)
		{
			_playerContacts--;
			if (_playerContacts == 0)
			{
				unschedule(schedule_selector(Wall::enableSound));
				scheduleOnce(schedule_selector(Wall::enableSound), 1.0f);
			}
		}
		Base::touchEnded(e, me, it);
	}

	void Wall::enableSound(float)
	{
		_soundEnabled = true;
	}

	Box::Box()
	{
		tintOnTouch = false;
		pushable = true;
		immortal = true;
	}

	void Box::createPhysics()
	{
		Base::createPhysics();
		body->SetType(b2_dynamicBody);
		body->GetFixtureList()->SetDensity(1.0f);
		body->GetFixtureList()->SetRestitution(0.2f);
	}

	bool Box::touch(Entity* e)
	{
		if (e->player)
		{
			int begin = 240;
			setDefaultColor(
				rand() % (255 - begin) + begin,
				rand() % (255 - begin) + begin,
				rand() % (255 - begin) + begin);
		}
		return Base::touch(e);
	}

	Interactive::Interactive()
	{
		tintOnTouch = false;
		pushable = false;
		listener = true;
		_shineR = 255;
		_shineG = 255;
		_shineB = 255;
		num = 0;
		op = ' ';
		_labelPresent = false;
	}

	void Interactive::startGameCall()
	{
		//updateLabel();
	}

	Label* Interactive::AddLabel(const string& _text)
	{
		auto label = Label::createWithSystemFont(_text, "Arial", 24.0f);
		auto spriteMid = 0.5f * getContentSize().width;
		label->setPositionX(spriteMid);
		label->setPositionY(spriteMid);
		label->setAnchorPoint(Point(0.5f, 0.5f));
		label->setHorizontalAlignment(TextHAlignment::CENTER);
		label->setVerticalAlignment(TextVAlignment::CENTER);
		addChild(label, 1);
		label->setTag(1);
		_labelPresent = true;
		return label;
	}

	Label* Interactive::AddLabel(void)
	{
		string text = nToString(op) + nToString(num);
		if (op == '|') text = nToString(num) + nToString(op);
		if (player) text = nToString(num);
		return AddLabel(text);
	}

	void Interactive::updateLabel(void)
	{
		return;
	}

	void Interactive::command(const string& line, int cpos)
	{
		CRASHONNULL;
		if (dead) return;
		string cmd = ReadBlock(line, cpos, ' ');
		if (cmd == "v")
			num = nToInt(ReadBlock(line, cpos));
		else if (cmd == "o")
			op = ReadBlock(line, cpos)[0];
	}

	bool Operation::init()
	{
		if (!Base::init()) return false;
		tintOnTouch = true;
		setShineColor(100, 100, 255);
		return true;
	}

	bool Operation::function()
	{
		CRASHONNULL2(false);

		sprite->runAction(Sequence::createWithTwoActions(MoveBy::create(0.1f, CP(0, 0.002f)),
			MoveBy::create(0.1f, CP(0.0f, -0.002f))));

		return true;
	}

	Once::Once()
	{
	}

	bool Once::touch(Entity* e)
	{
		//auto t = Base::touch(e);
		if (e->player)
		{
			if (function())
			{
				die();
				PlayEffect("bottle_pop_2.wav");
				return true;
			}
		}
		if (tintOnTouch) shine(_shineR, _shineG, _shineB);
		return false;
	}

	bool Condition::potentialPassable(Entity* e)
	{
		CRASHONNULL2(false);
		if (!e->player) return false;
		//return true if you want player to pass
		return false;
	}

	bool Condition::touch(Entity* e)
	{
		CRASHONNULL2(false);
		if (potentialPassable(e))
		{
			sprite->stopAllActions();
			shine(0, 255, 0, 0.1f, 2.0f);

			sprite->runAction(Sequence::createWithTwoActions(
				RotateBy::create(0.7f, 360.0f),
				RotateTo::create(0.1f, 0)));
			return true;
		}
		else
		{
			sprite->stopAllActions();
			shine(178, 45, 0);

			sprite->runAction(Sequence::createWithTwoActions(
				RotateBy::create(0.7f, -360.0f),
				RotateTo::create(0.1f, 0)));

			return false;
		}
	}

	bool OnceCondition::touch(Entity* e)
	{
		bool result = Base::touch(e);
		if (result) fadeDie(FADEDIETIME);
		return result;
	}

	void EntityWithItem::command(const string& line, int cpos)
	{
		string cmd = ReadBlock(line, cpos, ' ');
		if (cmd == "t") item = static_cast<InventoryItems>(nToInt(ReadBlock(line, cpos)));
	}

	bool Button::touch(Entity* e)
	{
		CRASHONNULL2(false);
		if (e->player)
		{
			log("Button is dying");
			bool killed = false;
			for (auto entity : Game->entities)
			{
				Door* d = dynamic_cast<Door*>(entity);
				if (d && d->item == item)
				{
					log("killing door");
					killed = true;
					d->fadeDie(FADEDIETIME);
				}
			}
			if (killed) PlayEffect("bottle_pop_2.wav", 0.5f);
			fadeDie(FADEDIETIME);
		}
		return true;
	}

	Door::Door()
	{
		pushed = false;
		listener = true;
	}

	bool Door::touch(Entity* e)
	{
		CRASHONNULL2(false);
		PressurePlate* pp = nullptr;
		for (auto entity : Game->entities)
		{
			pp = dynamic_cast<PressurePlate*>(entity);
			if (pp && pp->item == item &&
				static_cast<int>(Game->getEntities(pp->position).size()) > 1)
				return true;
		}
		shine(255, 0, 0);
		return false;
	}

	void Door::receiveMessage(Entity* e, const string& msg)
	{
		CRASHONNULL;
		if (dead) return;
		PressurePlate* pp = nullptr;
		bool found = false;
		for (auto entity : Game->entities)
		{
			pp = dynamic_cast<PressurePlate*>(entity);
			if (pp && pp->item == item &&
				static_cast<int>(Game->getEntities(pp->position).size()) > 1)
			{
				found = true;
				break;
			}
		}
		if (found)
		{
			if (!pushed)
			{
				pushed = true;
				sprite->runAction(FadeTo::create(0.3f, 20));
			}
		}
		else if (pushed)
		{
			pushed = false;
			sprite->runAction(FadeTo::create(0.3f, 255));
		}
	}

	void Teleport::createPhysics()
	{
		Base::createPhysics();
		//for (auto fix = body->GetFixtureList(); fix; fix = fix->GetNext())
		//fix->SetSensor(true);
	}

	void Teleport::touchEnded(Entity* e, b2Fixture*, b2Fixture* its)
	{
		if (e->player && (its->GetUserData()) == FIX_BASE)
		{
			for (auto entity : Game->entities)
			{
				TeleportDest* d = dynamic_cast<TeleportDest*>(entity);
				if (d && d->item == item)
				{
					Game->teleport(e, d->getPosition());
					break;
				}
			}
		}
	}

	void TeleportDest::createPhysics()
	{
		Base::createPhysics();
		b2Filter f;
		//f.maskBits = 0x0000;
		for (auto fix = body->GetFixtureList(); fix; fix = fix->GetNext())
			fix->SetSensor(true);
		//fix->SetFilterData(f);
	}

	Player::Player()
	{
		player = true;
		pusher = true;
		_jumpFlag = false;
		_angvel = 0;
	}

	bool Player::init()
	{
		if (!Base::init()) return false;
		/*auto p = ParticleGalaxy::create();
		p->setLife(1.0f);
		p->setPosition(Point::ZERO);
		p->setPositionType(ParticleSystem::PositionType::RELATIVE);
		p->setEndColor(Color4F(1.0f, 0.8f, 0.0f, 0.0f));
		p->setTag(TAG_PLAYER_PARTICLE);*/
		auto p = Node::create();
		addChild(p);
		scheduleUpdate();

		return true;
	}

	void Player::update(float dt)
	{
		sprite->setRotation(sprite->getRotation() + dt * _angvel);
		_angvel *= 0.99f;
	}

	void Player::die()
	{
		unscheduleUpdate();
		Entity::die();
		dead = true;
		//getChildByTag(TAG_PLAYER_PARTICLE)->runAction(ScaleTo::create(0.2f, 0.1f));
		//PlayEffect("playerdie.wav", 1.0f, 0.9f);
		Game->playerDied();
	}

	b2Fixture* Player::createShape(b2FixtureDef* blockShapeDef)
	{
		auto baseFix = createCircleShape(blockShapeDef);
		baseFix->SetRestitution(0.65f);
		//make feet sensor
		b2PolygonShape sensor;
		b2FixtureDef sensorDef;
		sensor.SetAsBox(
			0.9f * getContentSize().width / PTM_RATIO,
			0.1f * getContentSize().height / PTM_RATIO,
			b2Vec2(0, -getContentSize().height / PTM_RATIO), 0);

		sensorDef.shape = &sensor;
		sensorDef.isSensor = true;
		auto sensorFix = body->CreateFixture(&sensorDef);
		sensorFix->SetUserData(FIX_FOOT);

		return baseFix;
	}

	void Player::createPhysics()
	{
		Base::createPhysics();
		body->SetType(b2_dynamicBody);
		body->SetSleepingAllowed(false);
		body->SetFixedRotation(true);
		body->SetLinearDamping(0.5f);
	}

	bool Player::canJump()
	{
		//log(nToString(_jumpCount));
		return _jumpFlag;// || fabs(body->GetLinearVelocity().y) < 1.0f; // _jumpCount > 0;
	}

	void Player::disableJump(float)
	{
		_jumpFlag = false;
		sprite->setSpriteFrame("Platformer_ball_onair");
	}

	bool Player::touch(Entity* e, b2Fixture* my, b2Fixture* its)
	{
		if (_contacts.empty())
		{
			//PlayEffect("piano.wav", min(getPositionX() / Game->gameSize.width, 1.0f));
			//log("bb");
		}
		_contacts.insert(its);
		if (my->GetUserData() == FIX_FOOT)
		{
			_floors.insert(its);
			if (_jumpFlag)
			{
				sprite->setSpriteFrame("Platformer_ball");
			}
			_jumpFlag = true;
			unschedule(schedule_selector(Player::disableJump));
			//log("b " + nToString((int)_floors.size()));
		}
		return Base::touch(e, my, its);
	}

	void Player::touchEnded(Entity* e, b2Fixture* my, b2Fixture* its)
	{
		_contacts.erase(its);
		if (my->GetUserData() == FIX_FOOT)
		{
			_floors.erase(its);
			if (_floors.size() == 0)
			{
				//disableJump(0);
				unschedule(schedule_selector(Player::disableJump));
				scheduleOnce(schedule_selector(Player::disableJump), 0.35f);
			}
			//log("e " + nToString((int) _floors.size()));
		}
		Base::touchEnded(e, my, its);
	}


	void Player::move(Point p)
	{
		if (!canJump())
		{
			if (!Game->gravityFlipped() && p.y > 0) p.y = 0;
			else if (Game->gravityFlipped() && p.y < 0) p.y = 0;
		}
		body->ApplyLinearImpulse(b2Vec2(p.x * 0.1f, 0), body->GetWorldCenter(), true);
		//body->ApplyForceToCenter(b2Vec2(p.x, 0), true);
		auto currentVel = body->GetLinearVelocity();
		if (currentVel.x < -10) currentVel.x = -10;
		else if (currentVel.x > 10) currentVel.x = 10;

		if (Game->gravityFlipped())
		{
			body->SetLinearVelocity(b2Vec2(currentVel.x,
				p.y < 0 ?
				-PTM_RATIO * 0.5f :
				currentVel.y));
			if (p.y < 0) disableJump();
		}
		else
		{
			body->SetLinearVelocity(b2Vec2(currentVel.x,
				p.y > 0 ?
				PTM_RATIO * 0.5f :
				currentVel.y));
			if (p.y > 0) disableJump();
		}
		
		_angvel += 0.1f * currentVel.x;
		//log("aj: " + nToString(canJump()) + " p.y: " + nToString(p.y) + " v.y:" + nToString(currentVel.y) + " v.x:" + nToString(currentVel.x));
	}

	Water::Water()
	{
		power = 1.0f;
	}

	bool Water::init()
	{
		if (!Base::init()) return false;
		scheduleUpdate();
		return true;
	}
	b2Fixture* Water::createShape(b2FixtureDef* blockShapeDef)
	{
		auto baseFix = createCircleShape(blockShapeDef);
		baseFix->SetSensor(true);
		return baseFix;
	}

	bool Water::touch(Entity* e, b2Fixture* my, b2Fixture* it)
	{
		_contacts.insert(e);
		return Base::touch(e, my, it);
	}

	void Water::touchEnded(Entity* e, b2Fixture* my, b2Fixture* it)
	{
		_contacts.erase(e);
		Base::touchEnded(e, my, it);
	}

	void Water::update(float dt)
	{
		b2Vec2 force = -body->GetWorld()->GetGravity();
		force *= power;
		for (auto e : _contacts) e->body->ApplyForceToCenter(force, true);
	}

	bool Finish::touch(Entity* e)
	{
		Base::touch(e);
		if (e->player)
		{
			PlayEffect("start.wav");
			Game->gameOver(true);
			return true;
		}
		return false;
	}

	bool Gem::touch(Entity* e)
	{
		Base::touch(e);
		if (e->player)
		{
			die();
			return true;
		}
		return false;
	}

	b2Fixture* Eggplant::createShape(b2FixtureDef* blockShapeDef)
	{
		auto fix = createBoxShape(blockShapeDef);
		fix->SetSensor(true);
		return fix;
	}

	bool Eggplant::touch(Entity* e)
	{
		Base::touch(e);
		if (e->player)
		{
			Game->pickEggplant();
			die();
			return true;
		}
		return false;
	}

	bool Glass::touch(Entity* e)
	{
		Base::touch(e);
		if (e->player)
		{
			activateTimeBomb(0.3f);
			return true;
		}
		return false;
	}

	Hazard::Hazard()
	{
		harmful = true;
	}

	bool Hazard::touch(Entity* e)
	{
		Base::touch(e);
		if (harmful)
		{
			if (e->immortal) return true;

			if (dynamic_cast<Monster*>(e)) PlayEffect("monsterdie.wav");
			else if (dynamic_cast<Player*>(e)) PlayEffect("bottle_pop_2.wav", RANDF * 0.5f + 0.75f);
			e->die();
		}
		return true;
	}

	HPlatform::HPlatform()
	{
		d = 1;
		v = b2Vec2(1, 0);
		harmful = false;
	}

	bool HPlatform::init()
	{
		if (!Base::init()) return false;
		scheduleUpdate();
		return true;
	}

	b2Fixture* HPlatform::createShape(b2FixtureDef* blockShapeDef)
	{
		auto baseFix = createBoxShape(blockShapeDef);
		baseFix->SetRestitution(0.01f);
		baseFix->SetFriction(0.9f);
		return baseFix;
	}

	void HPlatform::createPhysics()
	{
		Base::createPhysics();
		body->SetType(b2_dynamicBody);
		body->SetGravityScale(0);		
		b2MassData massData;
		body->GetMassData(&massData);
		massData.mass = BIG_NUMBER;
		body->SetMassData(&massData);
		body->SetFixedRotation(true);
	}

	void HPlatform::update(float)
	{
		body->SetLinearVelocity(b2Vec2(v.x * d * 5.0f, v.y * d * 5.0f));

	}

	bool HPlatform::touch(Entity* e)
	{
		if (dynamic_cast<Wall*>(e)) d *= -1;// body->SetLinearVelocity(-body->GetLinearVelocity());
		return Base::touch(e);
	}

	VPlatform::VPlatform()
	{
		v = b2Vec2(0, 1);
	}

	bool Pit::touch(Entity* e)
	{
		Base::touch(e);
		if (e->player)
		{
			e->die();
			return true;
		}
		return false;
	}

	Monster::Monster()
	{
		harmful = true;
		pushable = true;
		interval = 0.15f;
		intelligence = 0.5f;
		actTimer = 0;
		currentMoveCount = 0;
		moveCount = 4;
		listener = true;
	}

	bool Monster::init()
	{
		if (!Entity::init())
		{
			return false;
		}

		schedule(schedule_selector(Monster::act), interval, kRepeatForever, RANDF * 4.0f);
		return true;
	}

	void Monster::act(float f)
	{
		CRASHONNULL;

		if (Game->state != GameStates::Ready) return;

		actTimer += f;
		if (actTimer > 1.0f)
		{
			if (currentMoveCount++ < moveCount)
			{
				seek(Game->player->position);
			}
			else
			{
				currentMoveCount = 0;
				actTimer = 0;
			}
		}
	}

	void Monster::die()
	{
		Base::die();
		Game->kills++;
		GameSettings::LastKills++;
		GameSettings::TotalKills++;
		if (GameSettings::TotalKills == 100)
			GameSettings::ReportAchievement(ACHIEVEMENT_DRONES_KILLER);
		GameSettings::DronesLastScore++;
	}

	void Monster::seek(Position target, bool advanced)
	{
		Directions d = static_cast<Directions>(rand() % 5);
		if (intelligence >= 1.0f || RANDF <= intelligence)
		{
			int dx = target.x - position.x;
			int dy = target.y - position.y;
			if (advanced)
			{
				auto d1 = dx < 0 ? Directions::Left : Directions::Right;
				auto d2 = dy < 0 ? Directions::Down : Directions::Up;
				if (dx == 0) d1 = Directions::Stop;
				if (dy == 0) d2 = Directions::Stop;
				auto dd = Directions::Stop;
				if (dx < 0 && dy < 0) dd = Directions::DL;
				if (dx > 0 && dy < 0) dd = Directions::DR;
				if (dx < 0 && dy > 0) dd = Directions::UL;
				if (dx > 0 && dy > 0) dd = Directions::UR;
				if (dd == Directions::Stop || !Game->move(this, dd))
				{
					if (RANDF < 0.5f) swap(d1, d2);
					Game->move(this, d1);
					Game->move(this, d2);
				}
			}
			else
			{
				//if in the same axis, move in the same axis
				//if not in same axis, only move diagonal
				auto dd = Directions::Stop;
				if (dx == 0) dd = dy < 0 ? Directions::Down : Directions::Up;
				else if (dy == 0) dd = dx < 0 ? Directions::Left : Directions::Right;
				else if (dx < 0 && dy < 0) dd = Directions::DL;
				else if (dx > 0 && dy < 0) dd = Directions::DR;
				else if (dx < 0 && dy > 0) dd = Directions::UL;
				else if (dx > 0 && dy > 0) dd = Directions::UR;
				if (dd != Directions::Stop)
					Game->move(this, dd);
			}
		}
	}

	bool Monster::touch(Entity* e)
	{
		Base::touch(e);
		if (e->player)
		{
			e->die();
			return true;
		}
		return false;
	}

	Robot::Robot()
	{
		intelligence = 1.0f;
	}

	void Robot::act(float d)
	{
		//Monster::act(d);
	}

	void Robot::receiveMessage(Entity* e, const string& msg)
	{
		Monster::receiveMessage(e, msg);
		if (e->player && msg == EMSG_MOVED) seek(e->position, false);
	}
}