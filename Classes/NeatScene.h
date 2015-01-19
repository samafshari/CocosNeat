#ifndef __NEAT_SCENE_H__
#define __NEAT_SCENE_H__

#include "helpers.h"

class NeatScene : public cocos2d::Layer
{
public:
    static cocos2d::Scene* createScene();
    virtual bool init();
    EventListenerTouchOneByOne* listener;
	// implement the "static create()" method manually
	CREATE_FUNC(NeatScene);

    
    bool onTouchBegan(Touch*, Event*);
    void onTouchEnded(Touch*, Event*);
    
private:
	void menuCloseCallback(Ref* pSender);
	void advanceScreen(float=0);
};

#endif // __NEAT_SCENE_H__
