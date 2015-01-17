#ifndef __DRONESGAMEOVER_SCENE_H__
#define __DRONESGAMEOVER_SCENE_H__

#include "helpers.h"

namespace Drones
{
	class GameOverScene : public cocos2d::Layer
	{
	public:
		static cocos2d::Scene* createScene();
		virtual bool init();

		CREATE_FUNC(GameOverScene);
		bool onTouchBegan(Touch*, Event*);

	private:
		void menuCloseCallback(Ref* pSender);
		void advanceScreen(float = 0);
	};
}

#endif
