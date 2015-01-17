#ifndef __GAME_INFO_SCENE__
#define __GAME_INFO_SCENE__

#include "helpers.h"
#include "GameSettings.h"

class GameInfoScene : public cocos2d::Layer
{
public:
	Games game;

    static cocos2d::Scene* createScene(Games);
    virtual bool init();  
	void createGamePanel();
	CREATE_FUNC(GameInfoScene);

    int oldGems;
    void checkGemsChanged(float);
private:
	void menuCloseCallback(Ref* pSender);
	void btnBuyPressed(Ref*);
	void onKeyReleased(EventKeyboard::KeyCode keyCode, Event* event) override;
    
};

#endif
