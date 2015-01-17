#include "Raid.h"
#include "ExtraGameOverScene.h"
#include "Ooze.h"

using namespace Raid;

const float Entity::SCALE = 0.3f;

Entity* Entity::create(string frameName)
{
	Entity* e = new Entity();
	if (e && e->init(frameName))
	{
		e->autorelease();
	}
	else
	{
		delete e;
		e = NULL;
	}
	return e;
}

bool Entity::init(string frameName)
{
	if (!initWithSpriteFrameName(frameName)) return false;
	setAnchorPoint(Point(0, 1));
	setScale(SCALE);

	noclip = false;
	vel = 0;
	wakeUpDistance = RANDF2(10, FRAME_SIZE.height); // FRAME_SIZE.height;
	scorePoints = 10;
	immortal = false;
	dead = false;
	collect = false;
	ghost = false;
	touched = false;
	return true;
}

ParticleSystemQuad* Entity::MakeExplosion()
{
    ParticleSystemQuad* p = ParticleSun::create();
    p->setSpeed(60);
    p->setStartSize(30);
    p->setEndSize(40);
    p->setStartColor(Color4F(0.76f, 0.25f, 0.12f, 0.1f));
    p->setPositionType(tPositionType::RELATIVE);
    p->setPosition(Point::ZERO);
    p->setLife(1.0f);
    p->setScale(4.0f);
    return p;
}

void Entity::die()
{
	if (dead) return;
	dead = true;
	runAction(FadeOut::create(0.4f));

	if (itemType != Items::Bridge)
	{
		PlayEffect("explosion.wav", RANDF * 0.3f + 0.7f);
		// ParticleSystemQuad::create("explosion/particle.plist");
        ParticleSystemQuad* p = Entity::MakeExplosion();
		addChild(p);
		

		auto cs = getContentSize();
		auto an = getAnchorPoint();
		auto sc = 1.0f;// getScale();
		p->setPositionX((0.5f - an.x)*cs.width*sc);
		p->setPositionY(-(0.5f - an.y)*cs.height*sc);

		p->setTag(kTAG_PARTICLE);
		scheduleOnce(schedule_selector(Entity::stopParticles), 0.5f);
		scheduleOnce(schedule_selector(Entity::performCollect), 2.0f);
	}
}

void Entity::stopParticles(float)
{
	static_cast<ParticleSystemQuad*>(getChildByTag(kTAG_PARTICLE))->stopSystem();
}

void Entity::performCollect(float)
{
	collect = true;
	removeChildByTag(kTAG_PARTICLE);
}

Entity* Entity::MakeEntity(Items itype, Point position, bool flip)
{
	Entity* e;
	switch (itype)
	{
	case Items::Fuel:
		e = Entity::create("fuel");
		e->vel = 0;
		e->wakeUpDistance = 0;
		e->scorePoints = 200;
		e->ghost = true;
		e->immortal = true;
		break;
	case Items::Heli:
	{
		e = Entity::create("heli1");
		e->vel = 4;
		e->noclip = false;
		e->scorePoints = 80;
		Vector<SpriteFrame*> f;
		f.pushBack(GETFRAME("heli2"));
		f.pushBack(GETFRAME("heli1"));
		auto anim = Animation::createWithSpriteFrames(f, 0.1f);
		e->runAction(RepeatForever::create(Animate::create(anim)));
		break;
	}
	case Items::House:
		e = Entity::create("house");
		e->vel = 0;
		e->noclip = true;
		e->immortal = true;
		break;
	case Items::Jet:
		e = Entity::create("jet");
		e->vel = 10;
		e->noclip = true;
		e->scorePoints = 120;
		//e->wakeUpDistance = DESIGN_Y;
		break;
	case Items::Ship:
		e = Entity::create("ship");
		e->vel = 3;
		e->noclip = false;
		e->scorePoints = 100;
		break;
	case Items::Tree:
		e = Entity::create("tree");
		e->vel = 0;
		e->noclip = true;
		e->immortal = true;
		break;
	case Items::Bridge:
		e = Entity::create("bridge");
		e->vel = 0;
		e->noclip = true;
		e->immortal = false;
		e->scorePoints = 500;
		e->setScale(SCALE * 2.0f);
		e->setAnchorPoint(Point(0.5f, 0));
		break;
	default:
		log("Incorrect entity type");
		return nullptr;
		//assert(false);
	}

	e->setPosition(position);
	if (flip)
	{
		e->setFlippedX(true);
		e->vel *= -1.0f;
	}
	e->itemType = itype;
	return e;
}

void Entity::moveX(float offset)
{
	setPositionX(getPositionX() + offset);
}

void Entity::moveY(float offset)
{
	setPositionY(getPositionY() + offset);
}
