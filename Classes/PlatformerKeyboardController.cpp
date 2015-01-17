#include "PlatformerKeyboardController.h"
#include "PlatformerScene.h"

using namespace std;
USING_NS_CC;

#define GETSCENE auto scene = (static_cast<PlatformerScene*>(getParent()))

namespace Platformer
{
	bool KeyboardController::init()
	{
		if (!InputController::init()) return false;

		auto listener = EventListenerKeyboard::create();
		listener->onKeyPressed = CC_CALLBACK_1(KeyboardController::onKeyPressed, this);
		listener->onKeyReleased = CC_CALLBACK_1(KeyboardController::onKeyReleased, this);
		this->getEventDispatcher()->addEventListenerWithSceneGraphPriority(listener, this);

		scheduleUpdate();

		return true;
	}

	void KeyboardController::onKeyPressed(Key_t keyCode)
	{
		GETSCENE;
		pressedKeys.insert(keyCode);

		if (keyCode == Key_t::KEY_UP_ARROW)
			scene->move(Point(0, 1));
		if (keyCode == Key_t::KEY_DOWN_ARROW)
			scene->game->flipGravity(!scene->game->gravityFlipped());
	}

	void KeyboardController::onKeyReleased(Key_t keyCode)
	{
		pressedKeys.erase(keyCode);
	}

	void KeyboardController::update(float)
	{
		GETSCENE;
		if (pressedKeys.end() != pressedKeys.find(Key_t::KEY_LEFT_ARROW))
			scene->move(Point(-3, 0));
		if (pressedKeys.end() != pressedKeys.find(Key_t::KEY_RIGHT_ARROW))
			scene->move(Point(3, 0));
	}
}