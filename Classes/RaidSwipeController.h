#ifndef __RAID_SWIPE_CONTROLLER_H__
#define __RAID_SWIPE_CONTROLLER_H__

#include "cocos2d.h"
USING_NS_CC;

#include "helpers.h"
#include "GameSettings.h"
#include "InputController.h"

namespace Raid
{
	class SwipeController : public InputController
	{
	public:
		virtual bool init();
		float angle;
		Point delta;
		CREATE_FUNC(SwipeController);

		bool onTouchBegan(Touch*, Event*) override;
		void onTouchMoved(Touch*, Event*) override;
		void onTouchEnded(Touch*, Event*) override;

		void sendData(float);

	private:
		virtual void reset() override;
	};
}

#endif
