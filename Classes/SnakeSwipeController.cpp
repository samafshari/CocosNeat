#include "SnakeSwipeController.h"
#include "SnakeScene.h"

using namespace Snake;
using namespace std;
USING_NS_CC;

#define GETGAME \
 	if (!getParent()) return; \
 	auto game = static_cast<SnakeLayer*>(static_cast<SnakeScene*>(getParent())->game)

bool SnakeSwipeController::init()
{
	if (!InputController::init()) return false;

	activeTouches = 0;

	auto listener = EventListenerTouchAllAtOnce::create();
	listener->onTouchesBegan = CC_CALLBACK_2(SnakeSwipeController::onTouchesBegan, this);
	listener->onTouchesMoved = CC_CALLBACK_2(SnakeSwipeController::onTouchesMoved, this);
	listener->onTouchesEnded = CC_CALLBACK_2(SnakeSwipeController::onTouchesEnded, this);
	getEventDispatcher()->addEventListenerWithSceneGraphPriority(listener, this);

	return true;
}

void SnakeSwipeController::onTouchesBegan(const std::vector<Touch*>& touches, Event*)
{   
	if (GETSCENE->dead) return;
    
	activeTouches += touches.size();

    swipeActive = false;
    swipeMoved = false;
	unschedule(schedule_selector(SnakeSwipeController::swipeMove));
    
	Touch* touch = *touches.rbegin();
    refTouchPosition = touch->getLocation();

	gettimeofday(&touchTime, NULL);
}

void SnakeSwipeController::onTouchesMoved(const std::vector<Touch*>& touches, Event*)
{
	if (GETSCENE->dead) return;
    
    Touch* touch = *touches.rbegin();
	Point delta = touch->getLocation() - touch->getStartLocation();

    bool oldSwipeActive = swipeActive;
    if (!swipeActive) swipeActive = delta.getLength() > 1.0f;
    if (swipeActive && !oldSwipeActive)
    {
        schedule(schedule_selector(SnakeSwipeController::swipeMove), 0.6f);
    }
    swipeDirection = Directions::Stop;
    float angle = CC_RADIANS_TO_DEGREES(delta.getAngle());
    float angleSlice = 0.5f * 360.0f / 4.0f;
        
    if (fabs(angle) < angleSlice) swipeDirection = Directions::Right;
    else if (fabs(angle) > 180.0f - angleSlice) swipeDirection = Directions::Left;
    else if (angle < 90.0f	+ angleSlice && angle > 90.0f	- angleSlice) swipeDirection = Directions::Up;
    else if (angle > -90.0f	- angleSlice && angle < -90.0f	+ angleSlice) swipeDirection = Directions::Down;
        
	refTouchPosition = touch->getLocation();
}

void SnakeSwipeController::onTouchesEnded(const std::vector<Touch*>& touches, Event*)
{
	if (GETSCENE->dead) return;
   
	activeTouches -= touches.size();
	if (activeTouches < 0) activeTouches = 0;

    if (swipeActive)
    {
        if (!swipeMoved)
            swipeMove();
        swipeActive = false;
        return;
    }
    
}

void SnakeSwipeController::swipeMove(float)
{
    if (!swipeActive)
    {
		unschedule(schedule_selector(SnakeSwipeController::swipeMove));
        return;
    }
    GETSCENE;
    if (!scene->dead)
    {
        if (swipeDirection != Directions::Stop)
        {
            swipeMoved = true;
            scene->move(swipeDirection);
        }
    }
}

void SnakeSwipeController::reset()
{
}