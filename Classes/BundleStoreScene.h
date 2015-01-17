#ifndef __BUNDLESTORESCENE_H__
#define __BUNDLESTORESCENE_H__

#include "helpers.h"

#define GEMS_TAG 10000
#define TIMER_TAG 10001
#define HOOKER_TAG 10002

class BundleStore : public cocos2d::Layer
{
public:
    virtual bool init();
    
    static Scene* createSceneWithText(string text);
    static Scene* createPreStoreScene();
    static Scene* createStoreScene();
    static Scene* createBuyingScene();
    static Scene* createBuyPassedScene();
    static Scene* createBuyFailedScene();
    static Scene* createRestorePassedScene(bool success);
    static Scene* createRestoringScene();
    
    static Node* createBackground();
	CREATE_FUNC(BundleStore);
	void menuCloseCallback(Ref*);
	void onKeyReleased(EventKeyboard::KeyCode keyCode, Event* event) override;
    void btnBuyPressed(Ref*);
    void btnRestorePressed(Ref*);
    void update(float) override;
    
    int lastGems;
    int lastTime;
#if IS_IOS
    vector<soomla::CCPurchasableVirtualItem *> buttonItems;
#endif
};

#endif
