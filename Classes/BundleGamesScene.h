#ifndef __BUNDLEGAMESSCENE_H__
#define __BUNDLEGAMESSCENE_H__

#include "helpers.h"

class BundleGamesScene : public cocos2d::Layer
{
public:
    virtual bool init();  
    static cocos2d::Scene* createScene();

	CREATE_FUNC(BundleGamesScene);
	void menuCloseCallback(Ref*);

	void btnDronesPressed(Ref*);
    
	void onKeyReleased(EventKeyboard::KeyCode keyCode, Event* event) override;
	void changeColor(float);

	static Scene* TryRunGame(Games, bool act=true);
	static void RunGame(Games);
};

#endif 
