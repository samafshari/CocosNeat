#ifndef __EPI_SCENE_H__
#define __EPI_SCENE_H__

#include "cocos2d.h"
USING_NS_CC;

class EpilogueScene : public cocos2d::Layer
{
public:
    static cocos2d::Scene* createScene();
    virtual bool init();  
	// implement the "static create()" method manually
	CREATE_FUNC(EpilogueScene);
	bool onTouchBegan(Touch*, Event*) override;
private:
	void menuCloseCallback(Ref* pSender);
	void changeColor(float = 0);
	void advanceScreen(float=0);
};

#endif // __NEAT_SCENE_H__
