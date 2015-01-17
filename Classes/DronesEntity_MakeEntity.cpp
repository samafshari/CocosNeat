#include "DronesEntity.h"
#include "helpers.h"

using namespace Drones;

Entity* Entity::MakeEntity(char charcode)
{
	Entity* e = nullptr;

	switch (charcode)
	{
	case '8':
	{
		e = Wall::create();
		e->sprite = e->AddSprite("solid");
		//e->setDefaultColor(200, 200, 200);
		float hue = (1.0f / (float) (1+GameSettings::LastWins));
		if (GameSettings::CurrentLevelId >= 0)
			hue = 0.7f * (float)(GameSettings::GetMaximumLevelId() - GameSettings::CurrentLevelId + 1) / (float)(GameSettings::GetMaximumLevelId());
		auto color = hsl2Rgb(RANDF * 0.4f - 0.2f + hue, 1.0f, 0.8f);
		//float base = RANDF2(0, 255);
		e->setDefaultColor(
			color.r,
			color.g,
			color.b
			);
		break;
	}

	case 'X':
		e = Finish::create();
		e->sprite = e->AddSprite("dronesGoal");
		e->sprite->setScale(0.9f);
		/*e->sprite->runAction(RepeatForever::create(Sequence::createWithTwoActions(
			ScaleTo::create(1.0f, 1.0f), ScaleTo::create(1.0f, 0.9f)
			))); */
		e->sprite->runAction(RepeatForever::create(Sequence::createWithTwoActions(
			TintTo::create(1.0f, 255, 255, 255), TintTo::create(1.0f, 255, 255, 0)
			)));
		e->sprite->runAction(RepeatForever::create(Sequence::createWithTwoActions(
			DelayTime::create(10.0f), RotateBy::create(1.0, -360)
			)));
		break;

	case '@':
		e = Player::create();
		e->sprite = e->AddSprite("player");
		e->sprite->setScale(0.9f);
		e->sprite->setColor(Color3B(
			GameSettings::PlayerR,
			GameSettings::PlayerG,
			GameSettings::PlayerB
			));
		e->initZ += 1;
		break;

	case 'o':
		e = Once::create();
		e->sprite = e->AddSprite("once");
		e->setDefaultColor(150, 0, 150);
		break;

	case 'g':
		e = Gem::create();
		e->sprite = e->AddSprite("gem");
		break;
	case 'r':
		e = Operation::create();
		e->sprite = e->AddSprite("repeat");
		e->setDefaultColor(0, 180, 217);
		break;

	case '?':
		e = Condition::create();
		e->sprite = e->AddSprite("condition");
		e->setDefaultColor(10, 10, 10);
		break;

	case '$':
		e = OnceCondition::create();
		e->sprite = e->AddSprite("condition1");
		e->setDefaultColor(10, 10, 10);
		break;

	case '+':
		e = Box::create();
		e->sprite = e->AddSprite("box");
		e->pusher = true;
		break;

	case '=':
		e = Box::create();
		e->sprite = e->AddSprite("box");
		break;
		
	case '{':
		e = Button::create();
		e->sprite = e->AddSprite("button");
		static_cast<Button*>(e)->item = InventoryItems::Key1;
		e->sprite->runAction(
			RepeatForever::create(Sequence::createWithTwoActions(
			ScaleTo::create(1.0f, 1.0f), ScaleTo::create(1.0f, 0.9f)
			)));
		e->setDefaultColor(200, 200, 0);
		break;

	case '(':
		e = Button::create();
		e->sprite = e->AddSprite("button");
		static_cast<Button*>(e)->item = InventoryItems::Key2;
		e->sprite->runAction(
			RepeatForever::create(Sequence::createWithTwoActions(
			ScaleTo::create(1.0f, 1.0f), ScaleTo::create(1.0f, 0.9f)
			)));
		e->setDefaultColor(200, 0, 0);
		break;

	case '[':
		e = Button::create();
		e->sprite = e->AddSprite("button");
		static_cast<Button*>(e)->item = InventoryItems::Key3;
		e->sprite->runAction(
			RepeatForever::create(Sequence::createWithTwoActions(
			ScaleTo::create(1.0f, 1.0f), ScaleTo::create(1.0f, 0.9f)
			)));
		e->setDefaultColor(200, 0, 200);
		break;

	case '<':
		e = Button::create();
		e->sprite = e->AddSprite("button");
		static_cast<Button*>(e)->item = InventoryItems::Key4;
		e->sprite->runAction(
			RepeatForever::create(Sequence::createWithTwoActions(
			ScaleTo::create(1.0f, 1.0f), ScaleTo::create(1.0f, 0.9f)
			)));
		e->setDefaultColor(0, 0, 200);
		break;

	case '1':
		e = PressurePlate::create();
		e->sprite = e->AddSprite("plate");
		static_cast<PressurePlate*>(e)->item = InventoryItems::Key1;
		e->setDefaultColor(255, 255, 50);
		e->initZ -= 1;
		break;

	case '2':
		e = PressurePlate::create();
		e->sprite = e->AddSprite("plate");
		static_cast<PressurePlate*>(e)->item = InventoryItems::Key2;
		e->setDefaultColor(255, 50, 50);
		e->initZ -= 1;
		break;

	case '3':
		e = PressurePlate::create();
		e->sprite = e->AddSprite("plate");
		static_cast<PressurePlate*>(e)->item = InventoryItems::Key3;
		e->setDefaultColor(255, 50, 255);
		e->initZ -= 1;
		break;

	case '4':
		e = PressurePlate::create();
		e->sprite = e->AddSprite("plate");
		static_cast<PressurePlate*>(e)->item = InventoryItems::Key4;
		e->setDefaultColor(50, 50, 255);
		e->initZ -= 1;
		break;

	case '}':
		e = Door::create();
		e->sprite = e->AddSprite("door");
		static_cast<Door*>(e)->item = InventoryItems::Key1;
		e->setDefaultColor(200, 200, 100);
		break;

	case ')':
		e = Door::create();
		e->sprite = e->AddSprite("door");
		static_cast<Door*>(e)->item = InventoryItems::Key2;
		e->setDefaultColor(200, 0, 0);
		break;

	case ']':
		e = Door::create();
		e->sprite = e->AddSprite("door");
		static_cast<Door*>(e)->item = InventoryItems::Key3;
		e->setDefaultColor(200, 0, 200);
		break;

	case '>':
		e = Door::create();
		e->sprite = e->AddSprite("door");
		static_cast<Door*>(e)->item = InventoryItems::Key4;
		e->setDefaultColor(0, 0, 200);
		break;

	case 't':
		e = Teleport::create();
		e->sprite = e->AddSprite("teleport");
		static_cast<Teleport*>(e)->item = InventoryItems::Teleport1;
		e->setDefaultColor(0, 0, 255);
		e->sprite->runAction(RepeatForever::create(Sequence::createWithTwoActions(
			TintTo::create(0.1f, 255, 255, 255), TintTo::create(0.1f, 0, 0, 255)
			)));
		e->sprite->runAction(RepeatForever::create(RotateBy::create(1.0f, 365.0f)));
		break;

	case 'T':
		e = Teleport::create();
		e->sprite = e->AddSprite("teleport");
		static_cast<Teleport*>(e)->item = InventoryItems::Teleport2;
		e->setDefaultColor(255, 0, 0);
		e->sprite->runAction(RepeatForever::create(Sequence::createWithTwoActions(
			TintTo::create(0.1f, 255, 255, 255), TintTo::create(0.1f, 255, 0, 0)
			)));
		e->sprite->runAction(RepeatForever::create(RotateBy::create(1.0f, 365.0f)));
		break;

	case 'd':
		e = TeleportDest::create();
		e->sprite = e->AddSprite("destination");
		static_cast<TeleportDest*>(e)->item = InventoryItems::Teleport1;
		e->setDefaultColor(50, 50, 255);
		//e->sprite->setOpacity(50);
		break;

	case 'D':
		e = TeleportDest::create();
		e->sprite = e->AddSprite("destination");
		static_cast<TeleportDest*>(e)->item = InventoryItems::Teleport2;
		e->setDefaultColor(255, 50, 50);
		//e->sprite->setOpacity(50);
		break;

	case 's':
		e = Eggplant::create();
		e->sprite = e->AddSprite("dronesGoal");
		break;

	case '%':
		e = Lolli::create();
		e->sprite = e->AddSprite("lolli");
		e->activateTimeBomb(6);
		break;

	case 'm':
	{
		e = Monster::create();
		e->sprite = e->AddSprite("robot");
		e->sprite->runAction(RepeatForever::create(RotateBy::create(3.0f, 365.0f)));
		e->sprite->runAction(
			RepeatForever::create(Sequence::createWithTwoActions(
			ScaleTo::create(1.0f, 1.0f), ScaleTo::create(1.0f, 0.9f)
			)));
		break;
	}

	case 'R':
	{
				e = Robot::create();
				if (GameSettings::CurrentEpisode > 0)
					e->sprite = e->AddSprite({ "robot", "robot", "robotf" });
				else
					e->sprite = e->AddSprite("robot");

				e->sprite->runAction(
					RepeatForever::create(Sequence::createWithTwoActions(
					ScaleTo::create(1.0f, 1.0f), ScaleTo::create(1.0f, 0.9f)
					)));
				break;
	}
	case '!':
	{
				e = Hazard::create();
				e->sprite = e->AddSprite("hazard");
				e->sprite->setScale(0.9f);
				e->sprite->runAction(RepeatForever::create(RotateBy::create(2.0f, 365.0f)));
				e->sprite->runAction(RepeatForever::create(Sequence::createWithTwoActions(
					TintTo::create(0.5f, 210, 255, 255), TintTo::create(0.5f, 255, 0, 0)
					)));
				break;
	}
	case 'O':
		e = Pit::create();
		break;

	case '.':
	{
				e = Fake::create();
				break;
	}
	default:
		break;
	}

	return e;
}