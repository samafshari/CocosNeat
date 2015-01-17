#ifndef __EXTRA_LOCKED_SCENE__
#define __EXTRA_LOCKED_SCENE__

#include "helpers.h"
#include "GameSettings.h"

class ExtraLockedScene : public cocos2d::Layer
{
public:
	Games game;

    static cocos2d::Scene* createScene(Games);
    virtual bool init();  
	void createGamePanel();
	CREATE_FUNC(ExtraLockedScene);

    int oldGems;
    void checkGemsChanged(float);
private:
	void menuCloseCallback(Ref* pSender);
	void btnBuyPressed(Ref*);
	void onKeyReleased(EventKeyboard::KeyCode keyCode, Event* event) override;
    
};

#endif
