#ifndef __MELODIESCENE_H__
#define __MELODIESCENE_H__

#include "helpers.h"

namespace Melodie
{
	class MelodieLayer;

	class MelodieScene : public Layer
	{
	public:
		MenuItemSprite* btnSound;
		MelodieLayer* game;

		virtual bool init();
		static cocos2d::Scene* scene();
		CREATE_FUNC(MelodieScene);

		void showGameOver();
		void btnBackPressed(Ref*);
		void switchToMainMenu(float = 0);

		void onKeyReleased(EventKeyboard::KeyCode keyCode, Event* event) override;
		
		static int LastScore;
		static int HighScore;
		static int TotalScore;

		static void LoadSettings();
		static void SaveSettings();

		void btnSoundPressed(Ref*);
	};
}

#endif