#include "BlobEntity.h"
#include "helpers.h"
#include "BlobLayer.h"
#include "Blob.h"

namespace Blob
{
	const float FADEDIETIME = 0.3f;
	const float BLOBSIZE = 100.0f;

	BlobLayer* Entity::Game = nullptr;

	vector<Color3B> Entity::Colors =
	{
		Color3B(100,100,255),
		Color3B(100,255,100),
		Color3B(255,200,100),
		Color3B(255,100,100),
		//Color3B(255,100,255),
		Color3B(255,255,255),
		Color3B(255,255,100),
		Color3B(100,255,255)
	};

	Entity::Entity()
	{
		position = Position{ 0, 0 };
		dead = false;
		collect = false;
		active = false;
		color = -1;
	}

	b2Fixture* Entity::createShape(b2FixtureDef* blockShapeDef)
	{
		return createBoxShape(blockShapeDef);
	}

	b2Fixture* Entity::createBoxShape(b2FixtureDef* blockShapeDef)
	{
		b2PolygonShape blockShape;
		blockShape.SetAsBox(
			/*0.98f **/ getContentSize().width / PTM_RATIO,
			/*0.98f **/ getContentSize().height / PTM_RATIO);

		blockShapeDef->shape = &blockShape;
		return body->CreateFixture(blockShapeDef);
	}

	b2Fixture* Entity::createCircleShape(b2FixtureDef* blockShapeDef)
	{
		b2CircleShape blockShape;
		blockShape.m_radius =
			0.97f * getContentSize().width / PTM_RATIO;

		blockShapeDef->shape = &blockShape;
		return body->CreateFixture(blockShapeDef);
	}

	void Entity::syncBodyPosition()
	{
		body->SetTransform(b2Vec2(
			(getPositionX()) / PTM_RATIO,
			(getPositionY()) / PTM_RATIO), body->GetAngle());
	}

	void Entity::makeSad(float)
	{
		if (dead || bomb || active || !happy) return;
		happy = false;
		mo->setFlippedY(true);
	}

	void Entity::makeHappy()
	{
		if (happy) return;
		PlayEffect(SOUNDS_HAPPY);
		mo->setFlippedY(false);
		scheduleOnce(schedule_selector(Entity::makeSad), 10.0f);
	}

	void Entity::start()
	{
		controllable = true;
		points = 1;
		happy = true;

		scheduleOnce(schedule_selector(Entity::disableControl), 5.0f);

		ring = MAKESPRITE("ring");
		ring->setAnchorPoint(Point(0.5f, 0.5f));
		ring->setPosition(Point::ZERO);
		ring->setScale(0.1f);
		addChild(ring);

		bomb = rand() % (Colors.size() * 3) == 0;
		if (bomb)
		{
			sprite = AddSprite("blob_bomb");
		}
		else
		{
			color = rand() % Colors.size();
			sprite = AddSprite("blob_face");
			sprite->setColor(Colors[color]);

			scheduleOnce(schedule_selector(Entity::makeSad), 10.0f);
		}

		alpha = MAKESPRITE("blob_alpha");
		sprite->addChild(alpha);
		float mid = sprite->getContentSize().width * 0.5f;
		alpha->setPosition(Point(mid, mid));

		auto cz = getContentSize();
		auto ss = sprite->getScale();

		float mo_y = RANDF2(0.25f, 0.35f);
		float eye_y = RANDF2(0.25f, 0.35f);
		float eye_x = RANDF2(0.25f, 0.35f);

		mo = MAKESPRITE(bomb ? "blob_m_bomb" : "blob_m_happy");
		addChild(mo, 10);
		mo->setPosition(Point(0, -cz.height * mo_y));
		mo->setScale(ss);

		eyel = MAKESPRITE("blob_eye");
		addChild(eyel);
		eyel->setPosition(Point(-cz.width * eye_x, cz.height * eye_y));
		eyel->setScale(ss);

		eyer = MAKESPRITE("blob_eye");
		addChild(eyer);
		eyer->setPosition(Point(cz.width * eye_x, cz.height * eye_y));
		eyer->setScale(ss);
		
		auto aabb = getBoundingBox();

		b2BodyDef blockBodyDef;
		blockBodyDef.type = b2_dynamicBody;
		blockBodyDef.position.Set(
			getPositionX() / PTM_RATIO,
			getPositionY() / PTM_RATIO);
		blockBodyDef.userData = this;
		body = Game->world->CreateBody(&blockBodyDef);
		body->SetLinearDamping(0.8f);

		// Create block shape
		b2FixtureDef blockShapeDef;
		blockShapeDef.density = 1.0;
		blockShapeDef.friction = 0.0f;
		blockShapeDef.restitution = 0.8f;
		blockShapeDef.userData = FIX_BASE;
		auto f = createShape(&blockShapeDef);
		f->SetUserData(FIX_BASE);
		body->SetUserData(this);

		schedule(schedule_selector(Entity::update), 0.2f);
	}

	Sprite* Entity::AddSprite(const vector<string>& _spriteNames)
	{
		return AddSprite(_spriteNames[rand() % _spriteNames.size()]);
	}

	Sprite* Entity::AddSprite(const string& _spriteName)
	{
		float blobsize = BLOBSIZE * RANDF2(0.9f, 1.10f);
		setContentSize(Size(0.5f + blobsize * 0.5f, 0.5f + blobsize * 0.5f));
		auto sprite = MAKESPRITE(_spriteName);
		auto spriteSize = sprite->getContentSize();
		sprite->setScale(blobsize / (spriteSize.width - 0.5f));
		addChild(sprite);
		sprite->setTag(0);
		return sprite;
	}

	void Entity::activateTimeBomb(float duration)
	{
		if (active) return;
		active = true;
		if (!bomb)
		{
			mo->setSpriteFrame("blob_m_shock");
			PlayEffect(SOUNDS_ACTIVATE, RANDF2(0.5f, 1.0f));
		}
		else
		{
		}
		unscheduleUpdate();
		scheduleOnce(schedule_selector(Entity::explode), duration);
		auto sc = sprite->getColor();
		auto blinkc = sc;
		blinkc.r += 100;
		blinkc.r = min(255, (int) blinkc.r);
		blinkc.g *= 0.75f;
		blinkc.b *= 0.75f;
		auto ss = sprite->getScale();
		sprite->runAction(RepeatForever::create(
			Sequence::createWithTwoActions(
			TintTo::create(0.2f, blinkc.r, blinkc.g, blinkc.b),
			TintTo::create(0.2f, sc.r, sc.g, sc.b)
			)));
		sprite->runAction(RepeatForever::create(
			Sequence::createWithTwoActions(
			ScaleTo::create(0.3f, ss * 0.9f),
			ScaleTo::create(0.3f, ss * 1.0f)
			)));
	}

	void Entity::explodeBomb()
	{
		for (auto neighbor : neighbors)
		{
			b2Vec2 v = neighbor->body->GetPosition() - body->GetPosition();
			v.Normalize();
			v *= 200.0f;
			neighbor->body->ApplyLinearImpulse(v, neighbor->body->GetPosition(), true);
		}
		activateTimeBomb(0.0f);
	}

	void Entity::die()
	{
		sprite->stopAllActions();

		if (bomb)
		{
			for (auto neighbor : neighbors)
			{
				neighbor->activateTimeBomb(2.0f);
			}
		}
		dead = true;
		for (auto child : getChildren())
		{
			child->runAction(FadeOut::create(FADEDIETIME * 0.5f));
		}
		alpha->runAction(FadeOut::create(FADEDIETIME * 0.25f));
		sprite->runAction(ScaleTo::create(FADEDIETIME, 0.01f));
		scheduleOnce(schedule_selector(Entity::doCollect), FADEDIETIME * 5.0f);
		unscheduleUpdate();
		b2Filter f;
		f.maskBits = 0x0000;
		for (auto fix = body->GetFixtureList(); fix; fix = fix->GetNext())
			fix->SetFilterData(f);
		body->SetGravityScale(0);

		Game->giveScore(points * (happy ? 2 : 1), this);
		PlayEffect(SOUND_SUCCESS, (float)(color+1)/(float)(Colors.size()));
/*
		if (!bomb)
		{
			Color4F sf = Color4F(Colors[color].r / 255.f, Colors[color].g / 255.f, Colors[color].b / 255.f, 0.6f);
			Color4F ef = Color4F(Colors[color].r / 255.f, Colors[color].g / 255.f, Colors[color].b / 255.f, 0.f);
			ParticleSystemQuad* p = ParticleSun::create();
			p->setStartColor(sf);
			p->setEndColor(ef);
			p->setPositionType(tPositionType::RELATIVE);
			p->setPosition(Point::ZERO);
			p->setScale(10);
			addChild(p);
		}*/
	}

	void Entity::doCollect(float)
	{
		stopAllActions();
		removeAllChildren();
		unscheduleAllSelectors();
		Game->world->DestroyBody(body);
		body = nullptr;
	}

	void Entity::disableControl(float)
	{
		controllable = false;
	}

	void Entity::fadeDie(float delay)
	{
		for (auto child : getChildren())
		{
			child->runAction(FadeOut::create(delay));
			child->runAction(ScaleTo::create(delay, 0.01f));
		}
		activateTimeBomb(delay);
	}

	void Entity::touch(Entity* e, b2Fixture* my, b2Fixture* its)
	{
		if (my->GetUserData() == FIX_AURA)
		{
			if (_contacts.empty())
			{
				//PlayEffect("piano.wav", min(getPositionX() / Game->gameSize.width, 1.0f));
				//log("bb");
			}
			_contacts.insert(its);
			neighbors.insert(e);

			ring->stopAllActions();
			ring->setScale(0.2f);
			ring->setOpacity(255);
			ring->runAction(Sequence::create(ScaleTo::create(0.25f, 1.0f), FadeOut::create(0.3f), NULL));

			if (!e->bomb) ring->setColor(Colors[e->color]);
			else ring->setColor(Color3B::BLACK);

			if (!bomb && e->color == color)
			{
				mates.insert(e);

				//if (mates >= 3 || e->mates >= 3 || e->active) activateTimeBomb(2.0f);
			}
		}
	}
	
	void Entity::touchEnded(Entity* e, b2Fixture* my, b2Fixture* its)
	{
		if (my->GetUserData() == FIX_AURA)
		{
			if (e->color == color) mates.erase(e);
			_contacts.erase(its);
			neighbors.erase(e);
		}
	}

	void Entity::update(float)
	{
		if (mates.size() >= 2 && !active) activateTimeBomb(2.5f);
		else
		for (auto mate : mates)
		{
			if (mate->active && !active)
			{
				points += mate->points;
				activateTimeBomb(3.0f);
				return;
			}
		}
	}

	Blob::Blob()
	{
	}

	bool Blob::init()
	{
		if (!Base::init()) return false;
		/*auto p = ParticleGalaxy::create();
		p->setLife(1.0f);
		p->setPosition(Point::ZERO);
		p->setPositionType(ParticleSystem::PositionType::RELATIVE);
		p->setEndColor(Color4F(1.0f, 0.8f, 0.0f, 0.0f));
		p->setTag(TAG_Blob_PARTICLE);*/
		auto p = Node::create();
		addChild(p);

		return true;
	}

	b2Fixture* Blob::createShape(b2FixtureDef* blockShapeDef)
	{
		auto baseFix = createCircleShape(blockShapeDef);

		b2FixtureDef auraDef;
		b2CircleShape auraShape;
		auraDef.userData = FIX_AURA;

		float auraSize = 1.15f;
		if (bomb) auraSize *= 1.5f;
		auraShape.m_radius = auraSize * getContentSize().width / PTM_RATIO;
		auraDef.shape = &auraShape;
		auraDef.isSensor = true;
		auto auraFix = body->CreateFixture(&auraDef);
		auraFix->SetUserData(FIX_AURA);

		return baseFix;
	}

	void Blob::start()
	{
		Base::start();
	}
}