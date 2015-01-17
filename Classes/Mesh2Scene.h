#ifndef __Mesh2Scene_H
#define __Mesh2Scene_H

#include "helpers.h"
#include "LineNode.h"

namespace Mesh2
{
	class Mesh2Scene : public Layer
	{
	public:
		virtual bool init();
		static cocos2d::Scene* scene();
		CREATE_FUNC(Mesh2Scene);
		Menu* pMenu;

		void btnBackPressed(Ref*);
		void onKeyReleased(EventKeyboard::KeyCode keyCode, Event* event) override;

		bool onTouchBegan(Touch*, Event*) override;
		void onTouchMoved(Touch*, Event*) override;
		void onTouchEnded(Touch*, Event*) override;

		void checkWin();
		void winGame();
		void loseGame(bool backPressed = true);
	};
}

#endif