#include <map>

#include "helpers.h"
#include "DronesEntity.h"
#include "DronesLayer.h"

using namespace Drones;

#define WALLCOLOR 255,255,255
#define WALLHITCOLOR 255, 50, 0
#define FADEDIETIME 0.3f
DronesLayer* Entity::Game = nullptr;

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

Entity::Entity()
{
	position = Position{ 0, 0 };
	player = false;
	harmful = false;
	pushable = false;
	pusher = false;
	dead = false;
	collect = false;
	_r = 255;
	_g = 255;
	_b = 255;
	initZ = 0;
	//moveToAction = nullptr;
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
	setContentSize(Size(DronesLayer::blockSize * 0.5f, DronesLayer::blockSize * 0.5f));
	auto sprite = MAKESPRITE(_spriteName);
	auto spriteSize = sprite->getContentSize();
	sprite->setScale(DronesLayer::blockSize / spriteSize.width);
	sprite->setPosition(DronesLayer::blockSize * 0.5f, DronesLayer::blockSize * 0.5f);
	addChild(sprite);
	sprite->setTag(0);
	return sprite;
}

void Entity::activateTimeBomb(float duration)
{
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
	setPosition(
		DronesLayer::blockSize * position.x, 
		DronesLayer::blockSize * position.y);
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

Wall::Wall()
{
	tintOnTouch = true;
}

bool Wall::touch(Entity* e)
{
	if (e->player && tintOnTouch)
	{
		shine(WALLHITCOLOR);
	}
	return false;
}

Box::Box()
{
	tintOnTouch = false;
	pushable = true;
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
	auto label = Label::createWithBMFont(MATH_FNT, _text);
	auto spriteMid = 0.5f * BLOCK_SCALE * Game->blockSize;
	label->setPositionX(spriteMid);
	label->setPositionY(spriteMid);
	label->setAnchorPoint(Point(0.5f,0.5f));
	label->setHorizontalAlignment(TextHAlignment::CENTER);
	label->setVerticalAlignment(TextVAlignment::CENTER);
	label->setScale(BLOCK_SCALE);
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
        for (auto entity : Game->entities)
        {
            Door* d = dynamic_cast<Door*>(entity);
            if (d && d->item == item)
            {
                log("killing door");
                d->fadeDie(FADEDIETIME);
            }
        }
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

bool Teleport::touch(Entity* e)
{
	CRASHONNULL2(false);
	if (e->player)
	{
		for (auto entity : Game->entities)
		{
			TeleportDest* d = dynamic_cast<TeleportDest*>(entity);
			if (d && d->item == item)
			{
				Game->teleport(e, d->position);
				break;
			}
		}
	}
	return false;
}

Player::Player()
{
	player = true;
	pusher = true;
	moveAnimation = nullptr;
}

void Player::die()
{
	Entity::die();
	PlayEffect("playerdie.wav", 1.0f, 0.9f);
	Game->playerDied();
}

void Player::moved(Position p)
{
	Entity::moved(p);

	Vector<SpriteFrame*> frames;
    
	if (p.x > position.x && p.y > position.y) sprite->setRotation(-45);
	else if (p.x < position.x && p.y < position.y) sprite->setRotation(-45);
	else if (p.x > position.x && p.y < position.y) sprite->setRotation(45);
	else if (p.x < position.x && p.y > position.y) sprite->setRotation(45);
	else sprite->setRotation(0);

	if (p.x > position.x) //moveleft
	{
		frames.pushBack(GETFRAME("manLt1"));
		frames.pushBack(GETFRAME("manLt2"));
		frames.pushBack(GETFRAME("manLt3"));
		frames.pushBack(GETFRAME("manLt4"));
	}
	else if (p.x < position.x) //movert
	{
		frames.pushBack(GETFRAME("manRt1"));
		frames.pushBack(GETFRAME("manRt2"));
		frames.pushBack(GETFRAME("manRt3"));
		frames.pushBack(GETFRAME("manRt4"));
	}
	else if (p.y > position.y) //movedn
	{
		frames.pushBack(GETFRAME("manDn1"));
		frames.pushBack(GETFRAME("manDn2"));
		frames.pushBack(GETFRAME("manDn3"));
		frames.pushBack(GETFRAME("manDn4"));
	}
	else if (p.y < position.y) //moveup
	{
		frames.pushBack(GETFRAME("manUp1"));
		frames.pushBack(GETFRAME("manUp2"));
		frames.pushBack(GETFRAME("manUp3"));
		frames.pushBack(GETFRAME("manUp4"));
	}
	if (moveAnimation != nullptr)
	{
		sprite->stopAction(moveAnimation);
	}
	auto anim = Animation::createWithSpriteFrames(frames, 0.1f);
	anim->setRestoreOriginalFrame(false);
	sprite->runAction(moveAnimation = Animate::create(anim));

	PlayEffect({ "move1.wav", "move2.wav", "move3.wav" }, 1.0f, 0.7f);
}

bool Player::touch(Entity* e)
{
	bool etouch = Entity::touch(e);
	if (e->harmful)
	{
		die();
		return true;
	}
	return etouch;
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
		Game->collectedGems++;
		die();
		return true;
	}
	return false;
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

bool Lolli::touch(Entity* e)
{
	Base::touch(e);
	if (e->player)
	{
		Game->pickLolli();
		die();
		return true;
	}
	return false;
}

void Lolli::timeBombExploded()
{
	Game->lolliDied();
	Base::timeBombExploded();
}

bool Hazard::touch(Entity* e)
{
	Base::touch(e);
	if (dynamic_cast<Monster*>(e)) PlayEffect("monsterdie.wav");
	else if (dynamic_cast<Player*>(e)) PlayEffect("suction2.wav");
	e->die();
	return true;
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
	interval = 0.4f;
	intelligence = 0.25f;
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
			seek(Game->player->position, false);
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
	CRASHONNULL;

	Directions d = static_cast<Directions>(rand() % 9);
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
	else Game->move(this, d);
		 
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