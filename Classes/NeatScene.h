#ifndef __NEAT_SCENE_H__
#define __NEAT_SCENE_H__

#include "helpers.h"

class NeatScene : public cocos2d::Layer
{
public:
    static cocos2d::Scene* createScene();
    virtual bool init();  
	// implement the "static create()" method manually
	CREATE_FUNC(NeatScene);

private:
	void menuCloseCallback(Ref* pSender);
	void advanceScreen(float);
};

#endif // __NEAT_SCENE_H__
