#ifndef __BlobSCENE_H__
#define __BlobSCENE_H__

#include "helpers.h"
#include "Box2D/Box2D.h"

namespace Blob
{
	class BlobLayer;

	class BlobScene : public Layer
	{
	public:
		Layer* background;
		BlobLayer* game;
		Node* pauseLayer;

		virtual bool init();
		static cocos2d::Scene* scene();
		CREATE_FUNC(BlobScene);

		void showGameOver();
		void btnBackPressed(Ref*);
		void switchToMainMenu(float = 0);

		bool onTouchBegan(Touch*, Event*);

		void onKeyReleased(EventKeyboard::KeyCode keyCode, Event* event) override;
		static int LastScore;
		static int HighScore;
		static int TotalScore;

		static void LoadSettings();
		static void SaveSettings();
		void btnPausedPressed(Ref*);

		Label* scoreLabel;
		void giveScore(int);
	};

	class PauseLayer : public Layer
	{
	public:
		CREATE_FUNC(PauseLayer);
		virtual bool init();
	};

	class BlobRainbowBackground : public Layer
	{
	public:
		CREATE_FUNC(BlobRainbowBackground);
		virtual bool init();

		Sprite *cloud1, *cloud2;
		void moveCloud1(float);
		void moveCloud2(float);
	};
}

#endif