#include "PlatformerSwipeController.h"
#include "PlatformerScene.h"
#include "PlatformerLayer.h"

#define FEEDBACKLAYER game->overlay
#define FEEDBACKTAG 10
using namespace std;
USING_NS_CC;

#define GETGAME \
 	if (!getParent()) return; \
 	auto game = static_cast<PlatformerLayer*>(static_cast<PlatformerScene*>(getParent())->game)

namespace Platformer
{
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
		vec->setAnchorPoint(Point(0, 0.5f));
		vec->setPosition(game->player->getPosition() + game->player->getChildByTag(0)->getPosition());
		vec->setOpacity(0);
		vec->runAction(FadeIn::create(0.25f));
		vec->setScaleX(0);
		vec->setTag(FEEDBACKTAG);
		FEEDBACKLAYER->addChild(vec);

		activeTouches += touches.size();

		swipeActive = false;
		swipeMoved = false;

		Touch* touch = *touches.rbegin();
		refTouchPosition = touch->getLocation();

		gettimeofday(&touchTime, NULL);
	}

	void SwipeController::onTouchesMoved(const std::vector<Touch*>& touches, Event*)
	{
		GETGAME;
		GETSCENE;
		if (scene->dead) return;
		if (game == nullptr || FEEDBACKLAYER == nullptr) return;
		if (game->player->getChildByTag(0) == nullptr) return;
		if (FEEDBACKLAYER->getChildByTag(FEEDBACKTAG) == nullptr) return;
		Touch* touch = *touches.rbegin();
		delta = touch->getLocation() - touch->getStartLocation();

		if (!swipeActive) swipeActive = delta.getLength() / game->blockSize > 1.0f;

		swipeDirection = Directions::Stop;
		float angle = CC_RADIANS_TO_DEGREES(delta.getAngle());
		FEEDBACKLAYER->getChildByTag(FEEDBACKTAG)->setRotation(-angle);
		FEEDBACKLAYER->getChildByTag(FEEDBACKTAG)->setPosition(game->player->getPosition() + game->player->getChildByTag(0)->getPosition());
		FEEDBACKLAYER->getChildByTag(FEEDBACKTAG)->setScaleX(clampf(delta.getLength() / game->blockSize, 0.0f, 1.7f));

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
		GETGAME;
		GETSCENE;
		if (!scene->dead && scene->game != nullptr && scene->game->state == GameStates::Ready)
		{
			if (delta.getLength() / game->blockSize > 1.0f)
			{
				scene->move(delta);
				FEEDBACKLAYER->removeChildByTag(FEEDBACKTAG);
			}
		}
	}

	void SwipeController::reset()
	{
	}
}