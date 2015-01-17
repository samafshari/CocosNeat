#ifndef __SNAKE_SWIPE_CONTROLLER_H__
#define __SNAKE_SWIPE_CONTROLLER_H__

#include "helpers.h"
#include "InputController.h"
#define GETSCENE auto scene = (static_cast<SnakeScene*>(getParent()))

namespace Snake
{
	class SnakeSwipeController : public InputController
	{
	public:
		virtual bool init();

		CREATE_FUNC(SnakeSwipeController);

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

		timeval touchTime;

		virtual void reset() override;
	};
}
#endif
