#ifndef __CreditsScene_SCENE_H__
#define __CreditsScene_SCENE_H__

#include "cocos2d.h"

class CreditsScene : public cocos2d::Layer
{
public:
	Label* text;
	Scene* previousScene;

	static void ShowCredits(string filename);

	CREATE_FUNC(CreditsScene);

	static Scene* createScene(string filename);
	virtual bool init() override;
	void createBody(string filename);
	void menuCloseCallback(Ref*);
	void onKeyReleased(EventKeyboard::KeyCode keyCode, Event* event) override;

	bool onTouchBegan(Touch*, Event*);
	void onTouchMoved(Touch*, Event*);
};

#endif // __CreditsScene_SCENE_H__
