#ifndef __PreGame_SCENE_H__
#define __PreGame_SCENE_H__

#include "cocos2d.h"
USING_NS_CC;

class PreGame : public cocos2d::Layer
{
public:
    virtual bool init();  
    static cocos2d::Scene* scene();
    
	void advance(float);

	void onKeyReleased(EventKeyboard::KeyCode keyCode, Event*);
	CREATE_FUNC(PreGame);
};

#endif
