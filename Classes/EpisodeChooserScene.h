#ifndef __EpisodeChooser_SCENE_H__
#define __EpisodeChooser_SCENE_H__

#include "cocos2d.h"
USING_NS_CC;

class EpisodeChooser : public cocos2d::Layer
{
	MenuItemSprite *btnSound;
public:
    virtual bool init();  
    static cocos2d::Scene* createScene();

	CREATE_FUNC(EpisodeChooser);
	void menuCloseCallback(Ref*);

	void btnEggplantPressed(Ref*);
	void btnArcadePressed(Ref*);
	void btnEpisodePressed(Ref*);
	void btnExtrasPressed(Ref*);

	void btnSoundPressed(Ref*);
	void onKeyReleased(EventKeyboard::KeyCode keyCode, Event* event) override;
	//void keyBackClicked();
};

#endif // __MainMenu_SCENE_H__
