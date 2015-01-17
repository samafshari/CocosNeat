#ifndef __MATRYOSHKASCENE_H__
#define __MATRYOSHKASCENE_H__

#include "helpers.h"

namespace Matryoshka
{
	class MatryoshkaLayer;

	const int MATRYOSHKA_MAX = 9; //0-9, inclusive
	
	class MatryoshkaScene : public Layer
	{
	public:
		static Scene* CreateMenu(Games);
		MenuItemSprite* btnSound;
		MatryoshkaLayer* game;

		virtual bool init();
		static cocos2d::Scene* scene();
		CREATE_FUNC(MatryoshkaScene);

		void showGameOver();
		void btnBackPressed(Ref*);
		void switchToMainMenu(float = 0);

		void onKeyReleased(EventKeyboard::KeyCode keyCode, Event* event) override;
		
		static int LastScore;
		static int HighScore;
		static int TotalScore;

		static void LoadSettings();
		static void SaveSettings();
	};
}

#endif