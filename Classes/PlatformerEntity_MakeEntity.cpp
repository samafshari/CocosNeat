#include "PlatformerEntity.h"
#include "helpers.h"

namespace Platformer
{
	Entity* Entity::MakeEntity(char charcode)
	{
		Entity* e = nullptr;

		switch (charcode)
		{
		case '8':
		{
			e = Wall::create();
			e->sprite = e->AddSprite("Platformer_wall");
			auto color = hsl2Rgb(RANDF, 1.0f, 0.7f);
			//auto color = Color3B::WHITE;
			e->setDefaultColor(
				color.r,
				color.g,
				color.b
				);
			break;
		}

		case 'X':
			e = Finish::create();
			e->sprite = e->AddSprite("star");
			e->sprite->runAction(RepeatForever::create(Sequence::createWithTwoActions(
				TintTo::create(1.0f, 255, 255, 255), TintTo::create(1.0f, 255, 255, 0)
				)));
			e->sprite->runAction(RepeatForever::create(Sequence::createWithTwoActions(
				DelayTime::create(10.0f), RotateBy::create(1.0, -360)
				)));
			break;

		case '@':
			e = Player::create();
			e->sprite = e->AddSprite("Platformer_ball");
			e->sprite->setColor(Color3B(
				GameSettings::PlayerR,
				GameSettings::PlayerG,
				GameSettings::PlayerB
				));
			e->initZ -= 1;
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
			e->setDefaultColor(200, 200, 0);
			break;

		case '(':
			e = Button::create();
			e->sprite = e->AddSprite("button");
			static_cast<Button*>(e)->item = InventoryItems::Key2;
			e->setDefaultColor(200, 0, 0);
			break;

		case '[':
			e = Button::create();
			e->sprite = e->AddSprite("button");
			static_cast<Button*>(e)->item = InventoryItems::Key3;
			e->setDefaultColor(200, 0, 200);
			break;

		case '<':
			e = Button::create();
			e->sprite = e->AddSprite("button");
			static_cast<Button*>(e)->item = InventoryItems::Key4;
			e->setDefaultColor(0, 0, 200);
			break;

		case '1':
			e = PressurePlate::create();
			e->sprite = e->AddSprite("plate");
			static_cast<PressurePlate*>(e)->item = InventoryItems::Key1;
			e->setDefaultColor(200, 200, 100);
			e->initZ -= 1;
			break;

		case '2':
			e = PressurePlate::create();
			e->sprite = e->AddSprite("plate");
			static_cast<PressurePlate*>(e)->item = InventoryItems::Key2;
			e->setDefaultColor(200, 100, 100);
			e->initZ -= 1;
			break;

		case '3':
			e = PressurePlate::create();
			e->sprite = e->AddSprite("plate");
			static_cast<PressurePlate*>(e)->item = InventoryItems::Key3;
			e->setDefaultColor(200, 100, 200);
			e->initZ -= 1;
			break;

		case '4':
			e = PressurePlate::create();
			e->sprite = e->AddSprite("plate");
			static_cast<PressurePlate*>(e)->item = InventoryItems::Key4;
			e->setDefaultColor(100, 100, 200);
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
			e->setDefaultColor(100, 100, 255);
			e->sprite->setOpacity(220);
			e->initZ -= 2;
			break;

		case 'D':
			e = TeleportDest::create();
			e->sprite = e->AddSprite("destination");
			static_cast<TeleportDest*>(e)->item = InventoryItems::Teleport2;
			e->setDefaultColor(255, 100, 100);
			e->sprite->setOpacity(220);
			e->initZ -= 2;
			break;

		case 'R':
		{
			e = Robot::create();
#ifdef FEMALE_ROBOT
			e->sprite = e->AddSprite({"robot", "robot", "robotf"});
#else
			e->sprite = e->AddSprite("robot");
#endif
			/*e->sprite->runAction(
				RepeatForever::create(Sequence::createWithTwoActions(
				ScaleBy::create(1.0f, 1.1f), ScaleBy::create(1.0f, 0.9f)
				)));*/
			break;
		}
		case '!':
		{
			e = Hazard::create();
			e->sprite = e->AddSprite("Platformer_spike");
			break;
		}
		case 'h':
		{
			e = HPlatform::create();
			e->sprite = e->AddSprite("Platformer_spike");
			e->harmful = true;
			break;
		}
		case 'v':
		{
			e = VPlatform::create();
			e->sprite = e->AddSprite("Platformer_spike");
			e->harmful = true;
			break;
		}
		case 'H':
		{
			e = HPlatform::create();
			e->sprite = e->AddSprite("Platformer_wall");
			break;
		}
		case 'V':
		{
			e = VPlatform::create();
			e->sprite = e->AddSprite("Platformer_wall");
			break;
		}
		case 'O':
			e = Pit::create();
			break;

		case 's':
			e = Eggplant::create();
			e->sprite = e->AddSprite("lolli");
			break;

		case '0':
			e = Glass::create();
			e->sprite = e->AddSprite("Platformer_glass");
			break;

		case 'w':
			e = Water::create();
			e->sprite = e->AddSprite("Platformer_water");
			e->sprite->setOpacity(100);
			//e->initZ -= 5;
			break;

		case 'W':
			e = Water::create();
			static_cast<Water*>(e)->power *= 1.4f;
			e->sprite = e->AddSprite("Platformer_water");
			e->sprite->setOpacity(125);
			//e->initZ -= 5;
			break;

		case '.':
		{
			e = Fake::create();
			break;
		}
		default:
			break;
		}

		if (e) e->charcode = charcode;
		return e;
	}
}