#ifndef __BundleMainMenu_SCENE_H__
#define __BundleMainMenu_SCENE_H__

#include "helpers.h"

class BundleMainMenu : public cocos2d::Layer
{
public:
    MenuItemSprite *btnSound;
    
    virtual bool init();  
    static cocos2d::Scene* scene();

	CREATE_FUNC(BundleMainMenu);
	void menuCloseCallback(Ref*);

	void btnNewGamePressed(Ref*);
	void btnStorePressed(Ref*);
	void btnExitPressed(Ref*);
	void btnScoresPressed(Ref*);
	void btnAchievementsPressed(Ref*);

	void btnSoundPressed(Ref*);
	void onKeyReleased(EventKeyboard::KeyCode keyCode, Event* event) override;
};

#endif
