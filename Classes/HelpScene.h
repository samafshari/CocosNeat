#ifndef __HelpScene_SCENE_H__
#define __HelpScene_SCENE_H__

#include "cocos2d.h"

class HelpScene : public cocos2d::Layer
{
public:
    static cocos2d::Scene* scene();
	virtual bool init();  
	
	CREATE_FUNC(HelpScene);

	void btnBackPressed(Ref*);

	bool onTouchBegan(Touch*, Event*) override;
	void onTouchMoved(Touch*, Event*) override;
	void onTouchEnded(Touch*, Event*) override;

	void onKeyReleased(EventKeyboard::KeyCode keyCode, Event* event) override;

	bool touched;
	int touchId;
	float initY;

	float touchY;
	float lastY;
	void updateLastY(float dt = 0);
};

#endif // __HelpScene_SCENE_H__
