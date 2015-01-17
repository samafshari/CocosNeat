#ifndef __ChooserScene_H__
#define __ChooserScene_H__

#include "helpers.h"

class ChooserScene : public cocos2d::Layer
{
public:
    virtual bool init(int offset=0);  
	static cocos2d::Scene* scene(int offset=0);
    
	CREATE_FUNC(ChooserScene);
	static ChooserScene* create(int offset=0);

	vector<Node*> blobs;
    vector<Node*> numbers;
    
	void btnBackPressed(Ref*);
	void btnNextPressed(Ref*);
	void btnMenuPressed(Ref*);
	void btnLevelPressed(Ref*);
	void bloop(float);
	
	void onKeyReleased(EventKeyboard::KeyCode keyCode, Event*) override;
};

#endif