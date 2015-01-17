#include "SnakeScene.h"
#include "helpers.h"
#include "MainMenuScene.h"
#include "SnakeLayer.h"
#include "ExtraGameOverScene.h"
#include "BackgroundWidget.h"
#include "Ooze.h"

#include "SnakeSwipeController.h"

namespace Snake
{
	Scene* SnakeScene::scene()
	{
		GameSettings::SnakePlayed++;
		GameSettings::ReportAchievement(ACHIEVEMENT_UNLOCKED_SNAKE);
		GameSettings::TryGiveUnlockAll();
		GameSettings::SaveSettings();
		Ooze::InstantSend({ { "Event", "GameStart" }, { "Game", "Snake" } });
		GameSettings::OozeGameStats();
		Scene *scene = Scene::create();
		SnakeScene *layer = SnakeScene::create();
		scene->addChild(layer);
		return scene;
	}

	bool SnakeScene::init()
	{
		if (!Layer::init()) return false;

		Size visibleSize = Director::getInstance()->getVisibleSize();

		auto bg = LayerColor::create(Color4B(10, 10, 255, 255), visibleSize.width, visibleSize.height);
		addChild(bg);

		Texture2D::TexParams texParams;
		texParams.magFilter = GL_LINEAR;
		texParams.minFilter = GL_LINEAR;
		texParams.wrapS = GL_REPEAT;
		texParams.wrapT = GL_REPEAT;
		Sprite* bgwrap = Sprite::create("snake_tile0.png", Rect(0, 0, visibleSize.width, visibleSize.height));
		bgwrap->getTexture()->setTexParameters(texParams);
		bgwrap->setPosition(CP(0.5f, 0.5f));
		addChild(bgwrap);

		addChild(BackgroundWidget::createWithoutBackground());

		dead = false;
		over = false;

		game = SnakeLayer::create();
		addChild(game);

		auto hooker = TopAnchor::create();
		addChild(hooker, 1);
		auto pMenu = Menu::create();
		pMenu->setPosition(Point::ZERO);
		hooker->addChild(pMenu);

#if (CC_TARGET_PLATFORM != CC_PLATFORM_ANDROID)
		auto btnBack = MAKEBUTTON("btnMenuUp", "btnMenuDn", btnBackPressed);
		pMenu->addChild(btnBack);
		btnBack->setPosition(VCP(0.0f, 1.0f));
		btnBack->setAnchorPoint(Point(-0.5f, 1.5f));
#endif

		auto keyboard = EventListenerKeyboard::create();
		keyboard->onKeyReleased = CC_CALLBACK_2(SnakeScene::onKeyReleased, this);
		getEventDispatcher()->addEventListenerWithSceneGraphPriority(keyboard, this);

		/*auto listener = EventListenerAcceleration::create(CC_CALLBACK_2(SnakeScene::onAcceleration, this));
		getEventDispatcher()->addEventListenerWithSceneGraphPriority(listener, this);*/


		addChild(SnakeSwipeController::create());
		origin = Point(0, -0.5);
		threshold = Size(0.1f, 0.1f);

		old = Point::ZERO;
		current = Point::ZERO;

		schedule(schedule_selector(SnakeScene::updateMove), 0.06f);

		return true;
	}

	void SnakeScene::onAcceleration(Acceleration* accel, Event*)
	{
		current.x = accel->x - origin.x;
		current.y = accel->y - origin.y;
	}

	void SnakeScene::updateMove(float)
	{
		if (dead) unscheduleAllSelectors();

		if (current == old) return;

		if (fabs(current.x) > fabs(current.y))
		{
			if (current.x > threshold.width) move(Directions::Right);
			else if (current.x < -threshold.width) move(Directions::Left);
		}
		else if (fabs(current.y) > fabs(current.x))
		{
			if (current.y > threshold.height) move(Directions::Up);
			else if (current.y < threshold.height) move(Directions::Down);
		}

		old = current;
	}

	bool SnakeScene::move(Directions dir)
	{
		if (over) return false;

		bool result;
		if ((result = game->move(dir)))
		{
		}


		return result;
	}

	void SnakeScene::btnBackPressed(Ref*)
	{
		switchToMainMenu();
	}

	void SnakeScene::showGameOver()
	{
		GameSettings::SnakeLastScore = game->score;
		GameSettings::SnakeTotalScore += GameSettings::SnakeLastScore;
		Director::getInstance()->replaceScene(TransitionFade::create(1.0f, 
			ExtraGameOverScene::createScene(Games::Snake)));
	}

	void SnakeScene::switchToMainMenu(float)
	{
		dead = true;
		over = true;
		
		showGameOver();
	}

	void SnakeScene::onKeyReleased(EventKeyboard::KeyCode keyCode, Event* event)
	{
		dead = true;
		if (keyCode == EventKeyboard::KeyCode::KEY_ESCAPE)
			btnBackPressed(nullptr);
	}
}