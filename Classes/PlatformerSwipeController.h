#ifndef __PLATFORMER_SWIPE_CONTROLLER_H__
#define __PLATFORMER_SWIPE_CONTROLLER_H__

#include "cocos2d.h"
USING_NS_CC;

#include "helpers.h"
#include "GameSettings.h"
#include "InputController.h"
#define GETSCENE auto scene = (static_cast<PlatformerScene*>(getParent()))
namespace Platformer
{
	class SwipeController : public InputController
	{
	public:
		virtual bool init();

		CREATE_FUNC(SwipeController);

		void onTouchesBegan(const std::vector<Touch*>& touches, Event*);
		void onTouchesMoved(const std::vector<Touch*>& touches, Event*);
		void onTouchesEnded(const std::vector<Touch*>& touches, Event*);

		void swipeMove(float dt = 0);

	private:
		int activeTouches;
		bool swipeActive;
		Directions swipeDirection;
		bool swipeMoved;
		Point refTouchPosition;
		Point refLayerPosition;
		Point delta;
		timeval touchTime;

		virtual void reset() override;
	};
}
#endif
