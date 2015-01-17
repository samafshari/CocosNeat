#ifndef __EPISODE_LOCKED_SCENE__
#define __EPISODE_LOCKED_SCENE__

#include "helpers.h"
#include "GameSettings.h"

class EpisodeLockedScene : public cocos2d::Layer
{
public:
    static cocos2d::Scene* createScene();
	virtual bool init();
	CREATE_FUNC(EpisodeLockedScene);

	int oldGems;
	void checkGemsChanged(float);
private:
	void menuCloseCallback(Ref* pSender);
	void btnBuyPressed(Ref*);
	void onKeyReleased(EventKeyboard::KeyCode keyCode, Event* event) override;
};

#endif
