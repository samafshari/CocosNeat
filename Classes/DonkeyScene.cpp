#include "DonkeyScene.h"
#include "ExtraGameOverScene.h"
#include "GameSettings.h"
#include "Ooze.h"

Scene* DonkeyScene::createScene()
{
    GameSettings::DonkeyPlayed++;
    GameSettings::ReportAchievement(ACHIEVEMENT_UNLOCKED_DONKEY);
    GameSettings::TryGiveUnlockAll();
    GameSettings::SaveSettings();
    Ooze::InstantSend({{"Event", "GameStart"}, {"Game", "Donkey"}});
    GameSettings::OozeGameStats();
	Scene* scene = Scene::create();
	auto layer = DonkeyScene::create();
	scene->addChild(layer);
	return scene;
}

bool DonkeyScene::init()
{
	if (!Layer::init()) return false;
	Size visibleSize = Director::getInstance()->getVisibleSize();

	auto bg = LayerColor::create(Color4B(0, 150, 100, 255), visibleSize.width, visibleSize.height);

	auto vig = Sprite::create("vig.png");
	auto vigSize = vig->getContentSize();
	vig->setScaleX(visibleSize.width / vigSize.width);
	vig->setScaleY(visibleSize.height / vigSize.height);
	vig->setPosition(CP(0.5f, 0.5f));
	addChild(vig);

	auto pMenu = Menu::create();
	pMenu->setPosition(Point::ZERO);
	addChild(pMenu);

#if (CC_TARGET_PLATFORM != CC_PLATFORM_ANDROID)
	auto btnBack = MAKEBUTTON("btnMenuUp", "btnMenuDn", die);
	pMenu->addChild(btnBack);
	btnBack->setPosition(VCP(0.0f, 1.0f));
    btnBack->setAnchorPoint(Point(-0.5f, 1.5f));
#endif

	auto keyboard = EventListenerKeyboard::create();
	keyboard->onKeyReleased = CC_CALLBACK_2(DonkeyScene::onKeyReleased, this);
	getEventDispatcher()->addEventListenerWithSceneGraphPriority(keyboard, this);

	auto listener = EventListenerAcceleration::create(CC_CALLBACK_2(DonkeyScene::onAcceleration, this));
	getEventDispatcher()->addEventListenerWithSceneGraphPriority(listener, this);
	
	scheduleUpdate();
	
	return true;
}

void DonkeyScene::onAcceleration(Acceleration* accel, Event*)
{
}

void DonkeyScene::onKeyReleased(EventKeyboard::KeyCode keyCode, Event* event)
{
	if (keyCode == EventKeyboard::KeyCode::KEY_ESCAPE)
		die();
}

void DonkeyScene::update(float dt)
{
}

void DonkeyScene::die(Ref*)
{
	unscheduleAllSelectors();
	GameSettings::DonkeyTotalScore += GameSettings::DonkeyLastScore;
	GameSettings::SaveSettings();
	Director::getInstance()->replaceScene(TransitionFade::create(1.0f, 
		ExtraGameOverScene::createScene(Games::Donkey)));
}