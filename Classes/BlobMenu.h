#ifndef __BLOBMENU_H__
#define __BLOBMENU_H__

#include "helpers.h"
#include "GameInfoScene.h"

class Banner;

namespace Blob
{
	class BlobMenu : public cocos2d::Layer
	{
	public:
		Games game;
		Banner* scoresBanner;

		static cocos2d::Scene* createScene(Games);
		virtual bool init();
		void createGamePanel();
		CREATE_FUNC(BlobMenu);
		void updateLabels(float);
		void gameFinished();
		vector<string> generateBannerStats();

	private:
		void menuCloseCallback(Ref* pSender);
		void btnAchievementsPressed(Ref*);
		void btnLeaderboardsPressed(Ref*);
		void btnBuyPressed(Ref*);
		void onKeyReleased(EventKeyboard::KeyCode keyCode, Event* event) override;
	};

	class BlobCredits : public cocos2d::Layer
	{
	public:
		Label* text;
		CREATE_FUNC(BlobCredits);

		static Scene* createScene();
		virtual bool init() override;
		void menuCloseCallback(Ref*);
		void onKeyReleased(EventKeyboard::KeyCode keyCode, Event* event) override;

		bool onTouchBegan(Touch*, Event*);
		void onTouchMoved(Touch*, Event*);
	};
}

#endif
