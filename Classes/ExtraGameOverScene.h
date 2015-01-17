#ifndef __EXTRAGAMEOVER_SCENE_H__
#define __EXTRAGAMEOVER_SCENE_H__

#include "helpers.h"
#include "cocos-ext.h"
#include "cocos/ui/CocosGUI.h"
using namespace ui;
using namespace cocos2d::extension;

#if !IS_IOS
#if !defined(DISABLE_SUBMIT)
#define LOCAL_NAMEBOX
#endif
#endif

class ExtraGameOverScene : public cocos2d::Layer
{
public:
    static cocos2d::Scene* createScene(Games game, Node* background=nullptr);
    virtual bool init();  
	void initHighScore();
	CREATE_FUNC(ExtraGameOverScene);
	bool onTouchBegan(Touch*, Event*);
	int score, highScore, totalScore;
	Games game;
	void onKeyReleased(EventKeyboard::KeyCode keyCode, Event* event);
	
	static function<void()> End;
	TextField* nameBox;
	//EditBox* nameBox;
	
private:
	void menuCloseCallback(Ref* pSender);
	void advanceScreen(float=0);
};

#endif
