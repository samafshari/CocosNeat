#include "Raid.h"
#include "ExtraGameOverScene.h"
#include "Ooze.h"

using namespace Raid;

bool Rocket::init()
{
	PlayEffect("rocket.wav", RANDF2(0.8f, 1.0f));
	speed = 500.0f;

	auto rocket = MAKESPRITE("fatrocket");
	auto projectile = Node::create();
	auto size = rocket->getContentSize();
	projectile->setContentSize(size);
	projectile->setAnchorPoint(Point(0.5f, 0));
	projectile->setTag(0);
	projectile->addChild(rocket);
	addChild(projectile);

	auto aabb = RaidLayer::CurrentGame->player->getBoundingBox();
	projectile->setPosition(Point(aabb.getMidX(), aabb.getMaxY() + aabb.size.height + size.height));
	
	auto steam = ParticleSystemQuad::create("steam/particle.plist");
	steam->setTag(kSTEAM_PARTICLE);
	steam->setPositionType(tPositionType::FREE);
	steam->setAnchorPoint(Point(0.5f, 1.0f));
	steam->setPosition(Point::ZERO);
	projectile->addChild(steam, -1);

	return true;
}

Rocket::Rocket() : moveWithPlayer(true)
{

}

void Rocket::hit(Node* projectile, bool wall)
{
	projectilesToDelete.push_back(projectile);
}

bool Rocket::removeInvisibleProjectiles()
{
	for (auto projectile : projectilesToDelete)
		removeChild(projectile);
	projectilesToDelete.clear();

	if (getChildrenCount() == 0) return false;
	if (getChildByTag(0)->getPositionY() > PY(1.0f))
	{
		removeChildByTag(0);
		return false;
	}

	return true;
}

void Rocket::update(float dt)
{
	if (!removeInvisibleProjectiles()) return;
	getChildByTag(0)->setPositionY(getChildByTag(0)->getPositionY() + speed * dt);
	if (moveWithPlayer)
		getChildByTag(0)->setPositionX(RaidLayer::CurrentGame->player->getPositionX());
}

bool Laser::init()
{
	PlayEffect("laser.wav", RANDF2(0.8f, 1.0f));
	speed = 900.0f;

	auto rocket = MAKESPRITE("laser");
	auto projectile = Node::create();
	projectile->setContentSize(rocket->getContentSize());
	projectile->setAnchorPoint(Point(0.5f, 0));
	projectile->setTag(0);
	projectile->addChild(rocket);
	projectile->setPosition(RaidLayer::CurrentGame->player->getPosition());
	addChild(projectile);

	return true;
}

void Laser::hit(Node* projectile, bool wall)
{
	if (wall) 
		Rocket::hit(projectile, wall);
}

bool NormalLaser::init()
{
	//PlayEffect("laser.wav", RANDF2(0.8f, 1.0f));
	speed = 900.0f;

	auto rocket = MAKESPRITE("laser");
	auto projectile = Node::create();
	projectile->setContentSize(rocket->getContentSize());
	projectile->setAnchorPoint(Point(0.5f, 0));
	projectile->setTag(0);
	projectile->addChild(rocket);
	projectile->setPosition(RaidLayer::CurrentGame->player->getPosition());
	addChild(projectile);
	moveWithPlayer = false;

	return true;
}

bool Gun::init()
{
	PlayEffect("gun.wav", RANDF2(0.8f, 1.0f));
	speed = 1200.0f;

	auto rocket = MAKESPRITE("bullet");
	auto projectile = Node::create();
	projectile->setContentSize(rocket->getContentSize());
	projectile->setAnchorPoint(Point(0.5f, 0));
	projectile->setTag(0);
	projectile->addChild(rocket);
	projectile->setPosition(RaidLayer::CurrentGame->player->getPosition());
	addChild(projectile);
	return true;
}

