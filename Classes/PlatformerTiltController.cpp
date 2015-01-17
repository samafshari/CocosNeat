#include "PlatformerTiltController.h"
#include "PlatformerScene.h"

using namespace std;
USING_NS_CC;

#define GETSCENE auto scene = (static_cast<PlatformerScene*>(getParent()))

namespace Platformer
{
	bool TiltController::init()
	{
		if (!InputController::init()) return false;

		auto listener = EventListenerAcceleration::create(CC_CALLBACK_2(TiltController::onAcceleration, this));
		getEventDispatcher()->addEventListenerWithSceneGraphPriority(listener, this);

		schedule(schedule_selector(TiltController::update), 0, kRepeatForever, 1.0f);

		return true;
	}

	void TiltController::onAcceleration(Acceleration* accel, Event*)
	{
		GETSCENE;
		scene->move(Point(accel->x * 20.0f, 0));
		if (/*lastV.y < 0 &&*/ accel->y > 0)
		{
			scene->game->flipGravity(true);
		}
		else if (/*lastV.y > 0 &&*/ accel->y < 0)
		{
			scene->game->flipGravity(false);
		}
		//log("tilt: " + nToString(FLOAT3(accel->x, accel->y, accel->z)));

		lastV = FLOAT3(accel->x, accel->y, accel->z);
	}

	void TiltController::update(float dt)
	{
	}
}