#ifndef __MINIMAINMENU_H__
#define __MINIMAINMENU_H__

#include "helpers.h"
#include "GameInfoScene.h"

class Banner;
class BannerRotator;
class HBar;

class MiniMainMenu : public cocos2d::Layer
{
public:
	Games game;
	Scene* previousScene;
	Banner* scoresBanner;
	Node* gameLogo;
	HBar* hbar;
	Node* rotParent;
	BannerRotator* rotator;
	MenuItemSprite *btnFacebook, *btnRate, *btnBuy;
	Menu* pMenu;

    static cocos2d::Scene* createScene(
		Games, 
		Scene* previous=nullptr, 
		Layer* background=nullptr);

    virtual bool init();  
	void createGamePanel();
	CREATE_FUNC(MiniMainMenu);
	void updateLabels(float);
	void gameFinished();

	void menuCloseCallback(Ref* pSender);
    void btnAchievementsPressed(Ref*);
    void btnLeaderboardsPressed(Ref*);
	vector<string> generateBannerStats();
	void btnBuyPressed(Ref*);
	void onKeyReleased(EventKeyboard::KeyCode keyCode, Event* event) override;
};

#endif
