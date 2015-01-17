#ifndef __SNAKESCENE_H__
#define __SNAKESCENE_H__

#include "helpers.h"

namespace Snake
{
	class SnakeLayer;

	class SnakeScene : public Layer
	{
	public:
		SnakeLayer* game;

		virtual bool init();
		static cocos2d::Scene* scene();
		CREATE_FUNC(SnakeScene);
		bool dead;

		void btnBackPressed(Ref*);
		void showGameOver();
		void switchToMainMenu(float = 0);

		void onKeyReleased(EventKeyboard::KeyCode keyCode, Event* event) override;
		bool move(Directions dir);
		bool over;

		Point current, old;
		Point origin;
		Size threshold;

		void onAcceleration(Acceleration* accel, Event*) override;
		void updateMove(float);
	};
}

#endif
