#ifndef __DronesKeyboardController_H
#define __DronesKeyboardController_H

#include "cocos2d.h"
USING_NS_CC;

#include "helpers.h"
#include "GameSettings.h"
#include "InputController.h"

namespace Drones
{
	class KeyboardController : public InputController
	{
	public:
		virtual bool init();

		CREATE_FUNC(KeyboardController);
		void onKeyPressed(EventKeyboard::KeyCode keyCode);
	};
}

#endif
