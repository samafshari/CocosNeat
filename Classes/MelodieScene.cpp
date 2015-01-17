#include "MelodieScene.h"
#include "MelodieLayer.h"
#include "MainMenuScene.h"
#include "ExtraGameOverScene.h"
#include "BackgroundWidget.h"
#include "Ooze.h"

namespace Melodie
{
	int MelodieScene::LastScore = 0;
	int MelodieScene::HighScore = 0;
	int MelodieScene::TotalScore = 0;

	Scene* MelodieScene::scene()
	{
		Ooze::InstantSend({ { "Event", "GameStart" }, { "Game", "Melodie" } });
		Scene *scene = Scene::create();
		MelodieScene *layer = MelodieScene::create();
		scene->addChild(layer);
		return scene;
	}

	bool MelodieScene::init()
	{
		if (!Layer::init()) return false;

		Size visibleSize = Director::getInstance()->getVisibleSize();

		auto bg = LayerColor::create(Color4B::BLACK, visibleSize.width, visibleSize.height);
		addChild(bg);

		//addChild(BackgroundWidget::createWithoutBackground());

		auto vig = Sprite::create("vig.png");
		auto vigSize = vig->getContentSize();
		vig->setScaleX(visibleSize.width / vigSize.width);
		vig->setScaleY(visibleSize.height / vigSize.height);
		vig->setPosition(CP(0.5f, 0.5f));
		addChild(vig);

		game = MelodieLayer::create();
		addChild(game);

		auto hooker = TopAnchor::create();
		addChild(hooker, 1);
		auto pMenu = Menu::create();
		pMenu->setPosition(Point::ZERO);
		hooker->addChild(pMenu);

#if (CC_TARGET_PLATFORM != CC_PLATFORM_ANDROID)
		auto btnBack = MAKEBUTTON("btnMenuUp", "btnMenuDn", btnBackPressed);
		pMenu->addChild(btnBack);
		btnBack->setPosition(CP(0.05f, 0.95f));
		//btnBack->setAnchorPoint(Point(-0.5f, 1.5f));
#endif

		/*string sprite = GameSettings::MuteSounds ? "btnMute" : "btnSound";
		string psprite = sprite + "Dn";
		sprite += "Up";

		btnSound = MAKEBUTTON(sprite, psprite, btnSoundPressed);
		pMenu->addChild(btnSound);
		btnSound->setPosition(CP(0.95f, 0.95f));*/

		auto keyboard = EventListenerKeyboard::create();
		keyboard->onKeyReleased = CC_CALLBACK_2(MelodieScene::onKeyReleased, this);
		getEventDispatcher()->addEventListenerWithSceneGraphPriority(keyboard, this);

		auto listener = EventListenerAcceleration::create(CC_CALLBACK_2(MelodieScene::onAcceleration, this));
		getEventDispatcher()->addEventListenerWithSceneGraphPriority(listener, this);
		schedule(schedule_selector(MelodieScene::update), 0, kRepeatForever, 1.0f);

#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)
		SendMessageWithParams(string("MoveBannerTopRight"), NULL);
#endif
		return true;
	}

	void MelodieScene::btnSoundPressed(Ref*)
	{
		GameSettings::MuteSounds = !GameSettings::MuteSounds;
		string sprite = GameSettings::MuteSounds ? "btnMute" : "btnSound";
		string psprite = sprite + "Dn";
		sprite += "Up";
		btnSound->setSelectedImage(MAKESPRITE(psprite.c_str()));
		btnSound->setNormalImage(MAKESPRITE(sprite.c_str()));
		PlayEffect("bottle_pop_2.wav");
		GameSettings::SaveSettings();
	}

	void MelodieScene::btnBackPressed(Ref*)
	{
		switchToMainMenu();
	}

	void MelodieScene::showGameOver()
	{
#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)
		SendMessageWithParams(string("MoveBannerTopCenter"), NULL);
#endif
		LastScore = game->score;
		TotalScore += LastScore;

		Director::getInstance()->replaceScene(TransitionFade::create(1.0f, 
			ExtraGameOverScene::createScene(Games::Melodie)));

		HighScore = max(LastScore, HighScore);
		SaveSettings();
	}

	void MelodieScene::switchToMainMenu(float)
	{
		showGameOver();
	}

	void MelodieScene::LoadSettings()
	{
		auto d = UserDefault::getInstance();
		LastScore = d->getIntegerForKey("MelodieLastScore", LastScore);
		HighScore = d->getIntegerForKey("MelodieHighScore", HighScore);
		TotalScore = d->getIntegerForKey("MelodieTotalScore", TotalScore);
	}

	void MelodieScene::SaveSettings()
	{
		auto d = UserDefault::getInstance();
		d->setIntegerForKey("MelodieLastScore", LastScore);
		d->setIntegerForKey("MelodieHighScore", HighScore);
		d->setIntegerForKey("MelodieTotalScore", TotalScore);
		GameSettings::SaveSettings();
	}

	void MelodieScene::onKeyReleased(EventKeyboard::KeyCode keyCode, Event* event)
	{
		if (keyCode == EventKeyboard::KeyCode::KEY_ESCAPE)
			btnBackPressed(nullptr);
	}
}