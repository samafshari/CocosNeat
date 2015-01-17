#include "RaidSwipeController.h"
#include "Raid.h"

using namespace std;
USING_NS_CC;

#define GETSCENE auto scene = (static_cast<RaidScene*>(getParent()))
#define GETGAME \
 	if (!getParent()) return; \
 	auto game = static_cast<RaidLayer*>(static_cast<RaidScene*>(getParent())->game)

using namespace Raid;
bool SwipeController::init()
{
	if (!InputController::init()) return false;

	auto listener = EventListenerTouchOneByOne::create();
	listener->onTouchBegan = CC_CALLBACK_2(SwipeController::onTouchBegan, this);
	listener->onTouchMoved = CC_CALLBACK_2(SwipeController::onTouchMoved, this);
	listener->onTouchEnded = CC_CALLBACK_2(SwipeController::onTouchEnded, this);
	listener->onTouchCancelled = CC_CALLBACK_2(SwipeController::onTouchCancelled, this);
	getEventDispatcher()->addEventListenerWithSceneGraphPriority(listener, this);

	return true;
}

bool SwipeController::onTouchBegan(Touch* touch, Event*)
{
	return true;
}

void SwipeController::onTouchMoved(Touch* touch, Event*)
{
	GETSCENE;
	scene->teleport(touch->getDelta().x);
	delta = touch->getLocation() - touch->getStartLocation();
	angle = CC_RADIANS_TO_DEGREES(delta.getAngle());

	float thresholdX = PX(0.2f);
	float thresholdY = PY(0.2f);
	float x = delta.x >= 0 ? min(delta.x, thresholdX) : -min(-delta.x, thresholdX);
	float y = delta.y >= 0 ? min(delta.y, thresholdY) : -min(-delta.y, thresholdY);
	y = 0.5f + (y * 0.25f);
	y += 0.5f;
	scene->boost(y);
}

void SwipeController::onTouchEnded(Touch* touch, Event*)
{   
	// unschedule(schedule_selector(SwipeController::sendData));
}

void SwipeController::reset()
{
}