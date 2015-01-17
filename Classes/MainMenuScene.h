#ifndef __MainMenu_SCENE_H__
#define __MainMenu_SCENE_H__

#include "cocos2d.h"
USING_NS_CC;

class MainMenu : public cocos2d::Layer
{
public:
    virtual bool init();  
    static cocos2d::Scene* scene();

	CREATE_FUNC(MainMenu);
	void menuCloseCallback(Ref*);

	void btnNewGamePressed(Ref*);
	void btnHelpPressed(Ref*);
	void btnCreditsPressed(Ref*);
	void btnExitPressed(Ref*);
	void btnScoresPressed(Ref*);
	void btnAchievementsPressed(Ref*);
    void btnExtrasPressed(Ref*);
    
	void btnSoundPressed(Ref*);
	void onKeyReleased(EventKeyboard::KeyCode keyCode, Event* event) override;
};

#endif // __MainMenu_SCENE_H__
