#ifndef __DRONES_SPLASH_SCENE__
#define __DRONES_SPLASH_SCENE__

#include "helpers.h"

class DronesSplashScene : public cocos2d::Layer
{
public:
    static cocos2d::Scene* createScene();
	virtual bool init();
	CREATE_FUNC(DronesSplashScene);

	void menuCloseCallback(Ref* pSender);
	bool onTouchBegan(Touch*, Event*);
	void onKeyReleased(EventKeyboard::KeyCode keyCode, Event* event) override;
};

#endif
