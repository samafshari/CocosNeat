#include "DronesSwipeController.h"
#include "DronesScene.h"
#include "DronesLayer.h"
#define FEEDBACKLAYER game->overlay
#define FEEDBACKTAG 10

using namespace std;
USING_NS_CC;
using namespace Drones;

#define GETGAME \
 	if (!getParent()) return; \
 	auto game = static_cast<DronesLayer*>(static_cast<DronesScene*>(getParent())->game)

bool SwipeController::init()
{
	if (!InputController::init()) return false;

	activeTouches = 0;

	auto listener = EventListenerTouchAllAtOnce::create();
	listener->onTouchesBegan = CC_CALLBACK_2(SwipeController::onTouchesBegan, this);
	listener->onTouchesMoved = CC_CALLBACK_2(SwipeController::onTouchesMoved, this);
	listener->onTouchesEnded = CC_CALLBACK_2(SwipeController::onTouchesEnded, this);
	getEventDispatcher()->addEventListenerWithSceneGraphPriority(listener, this);

	return true;
}

void SwipeController::onTouchesBegan(const std::vector<Touch*>& touches, Event*)
{
	GETGAME;
    GETSCENE;
    if (game == nullptr || FEEDBACKLAYER == nullptr) return;
	FEEDBACKLAYER->removeChildByTag(FEEDBACKTAG);
	if (scene->dead) return;
    if (game->player->dead) return;
    
	auto vec = MAKESPRITE("arrSwipe");
    vec->setAnchorPoint(Point(0,0.5f));
    vec->setPosition(game->player->getPosition() + game->player->getChildByTag(0)->getPosition());
    vec->setOpacity(0);
    vec->runAction(FadeIn::create(0.25f));
    vec->setScaleX(0);
    vec->setTag(FEEDBACKTAG);
    FEEDBACKLAYER->addChild(vec);
    
	activeTouches += touches.size();

    swipeActive = false;
    swipeMoved = false;
	unschedule(schedule_selector(SwipeController::swipeMove));
    
	Touch* touch = *touches.rbegin();
    refTouchPosition = touch->getLocation();

	gettimeofday(&touchTime, NULL);
}

void SwipeController::onTouchesMoved(const std::vector<Touch*>& touches, Event*)
{
	GETGAME;
    GETSCENE;
	if (scene->dead) return;
    if (game == nullptr) return;
	if (game->player->getChildByTag(0) == nullptr) return;
    Touch* touch = *touches.rbegin();
	Point delta = touch->getLocation() - touch->getStartLocation();

    bool oldSwipeActive = swipeActive;
    if (!swipeActive) swipeActive = delta.getLength() / game->blockSize > 1.0f;
    if (swipeActive && !oldSwipeActive)
    {
        schedule(schedule_selector(SwipeController::swipeMove), 0.2f, kRepeatForever, 0.4f);
    }
    swipeDirection = Directions::Stop;
    float angle = CC_RADIANS_TO_DEGREES(delta.getAngle());

	if (FEEDBACKLAYER != nullptr && FEEDBACKLAYER->getChildByTag(FEEDBACKTAG) != nullptr)
	{
		FEEDBACKLAYER->getChildByTag(FEEDBACKTAG)->setRotation(-angle);
		FEEDBACKLAYER->getChildByTag(FEEDBACKTAG)->setPosition(game->player->getPosition() + game->player->getChildByTag(0)->getPosition());
		FEEDBACKLAYER->getChildByTag(FEEDBACKTAG)->setScaleX(clampf(delta.getLength() / game->blockSize, 0.0f, 1.7f));
	}
	//log("swipe angle:" + nToString(angle));
    float angleSlice = 0.5f * 360.0f / 8.0f;
        
    if (fabs(angle) < angleSlice) swipeDirection = Directions::Right;
    else if (fabs(angle) > 180.0f - angleSlice) swipeDirection = Directions::Left;
    else if (angle > 90.0f	+ angleSlice && angle < 180.0f	- angleSlice) swipeDirection = Directions::UL;
    else if (angle < -90.0f	- angleSlice && angle > -180.0f	+ angleSlice) swipeDirection = Directions::DL;
    else if (angle < 90.0f	+ angleSlice && angle > 90.0f	- angleSlice) swipeDirection = Directions::Up;
    else if (angle > -90.0f	- angleSlice && angle < -90.0f	+ angleSlice) swipeDirection = Directions::Down;
    else if (angle > 0)		swipeDirection = Directions::UR;
    else					swipeDirection = Directions::DR;
        
	refTouchPosition = touch->getLocation();
}

void SwipeController::onTouchesEnded(const std::vector<Touch*>& touches, Event*)
{
	GETGAME;
    GETSCENE;
	if (scene->dead) return;
    if (game == nullptr || FEEDBACKLAYER == nullptr) return;
    FEEDBACKLAYER->removeChildByTag(FEEDBACKTAG);
    
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

void SwipeController::swipeMove(float)
{
    if (!swipeActive)
    {
		unschedule(schedule_selector(SwipeController::swipeMove));
        return;
    }
    GETSCENE;
    if (!scene->dead && scene->game != nullptr && scene->game->state == GameStates::Ready)
    {
        if (swipeDirection != Directions::Stop)
        {
            swipeMoved = true;
            scene->move(swipeDirection);
            GETGAME;
            FEEDBACKLAYER->removeChildByTag(FEEDBACKTAG);
        }
    }
}

void SwipeController::reset()
{
}