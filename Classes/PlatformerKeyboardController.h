#ifndef __PlatformerKeyboardController_H
#define __PlatformerKeyboardController_H

#include "cocos2d.h"
USING_NS_CC;

#include "helpers.h"
#include "GameSettings.h"
#include "InputController.h"

namespace Platformer
{
	typedef EventKeyboard::KeyCode Key_t;

	class KeyboardController : public InputController
	{
	public:
		virtual bool init();

		CREATE_FUNC(KeyboardController);
		
		set<Key_t> pressedKeys;

		void update(float);
		void onKeyPressed(Key_t keyCode);
		void onKeyReleased(Key_t keyCode);
	};
}
#endif
