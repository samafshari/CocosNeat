#include "DronesKeyboardController.h"
#include "DronesScene.h"

using namespace std;
USING_NS_CC;
using namespace Drones;

#define GETSCENE auto scene = (static_cast<DronesScene*>(getParent()))

bool KeyboardController::init()
{
	if (!InputController::init()) return false;

	auto listener = EventListenerKeyboard::create();
	listener->onKeyPressed = CC_CALLBACK_1(KeyboardController::onKeyPressed, this);
	this->getEventDispatcher()->addEventListenerWithSceneGraphPriority(listener, this);

	return true;
}

void KeyboardController::onKeyPressed(EventKeyboard::KeyCode keyCode)
{
	GETSCENE;
	if (keyCode == EventKeyboard::KeyCode::KEY_LEFT_ARROW || keyCode == EventKeyboard::KeyCode::KEY_A) scene->move(Directions::Left);
	if (keyCode == EventKeyboard::KeyCode::KEY_RIGHT_ARROW || keyCode == EventKeyboard::KeyCode::KEY_D) scene->move(Directions::Right);
	if (keyCode == EventKeyboard::KeyCode::KEY_UP_ARROW || keyCode == EventKeyboard::KeyCode::KEY_W) scene->move(Directions::Up);
	if (keyCode == EventKeyboard::KeyCode::KEY_DOWN_ARROW || keyCode == EventKeyboard::KeyCode::KEY_X) scene->move(Directions::Down);
	if (keyCode == EventKeyboard::KeyCode::KEY_E) scene->move(Directions::UR);
	if (keyCode == EventKeyboard::KeyCode::KEY_C) scene->move(Directions::DR);
	if (keyCode == EventKeyboard::KeyCode::KEY_Q) scene->move(Directions::UL);
	if (keyCode == EventKeyboard::KeyCode::KEY_Z) scene->move(Directions::DL);
	if (keyCode == EventKeyboard::KeyCode::KEY_S) scene->move(Directions::Stop);
	if (keyCode == EventKeyboard::KeyCode::KEY_R) scene->btnRefreshPressed(this);
}