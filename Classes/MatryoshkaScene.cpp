#include "MatryoshkaScene.h"
#include "MatryoshkaLayer.h"
#include "MiniMainMenu.h"
#include "ExtraGameOverScene.h"
#include "BackgroundWidget.h"
#include "Ooze.h"
#include "Widgets.h"
#include "Sidebar.h"

namespace Matryoshka
{
	int MatryoshkaScene::LastScore = 0;
	int MatryoshkaScene::HighScore = 0;
	int MatryoshkaScene::TotalScore = 0;
	Scene* MatryoshkaScene::CreateMenu(Games game)
	{
		Layer* layer = Layer::create();
		auto bgcolor = LayerColor::create(GameInfos[game].color, DESIGN_X, DESIGN_Y);
		bgcolor->setPosition(Point::ZERO);
		layer->addChild(bgcolor);

		auto bigmat = Sprite::create("bigmat.png");
		bigmat->setAnchorPoint(Point(0.2f, 0.1f));
		bigmat->setScale(2.0f);
		layer->addChild(bigmat);

		layer->addChild(Vignette::create());

		return MiniMainMenu::createScene(game, nullptr, layer);
	}

	Scene* MatryoshkaScene::scene()
	{
		Ooze::InstantSend({ { "Event", "GameStart" }, { "Game", GameInfos[ Games::Matryoshka ].title } });
		Scene *scene = Scene::create();
		MatryoshkaScene *layer = MatryoshkaScene::create();
		scene->addChild(layer);
		return scene;
	}

	bool MatryoshkaScene::init()
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
		Sprite* bgwrap = Sprite::create("matryoshka_tile0.png", Rect(0, 0, visibleSize.width, visibleSize.height));
		bgwrap->getTexture()->setTexParameters(texParams);
		bgwrap->setPosition(CP(0.5f, 0.5f));
		addChild(bgwrap);

		addChild(BackgroundWidget::createWithoutBackground());

		game = MatryoshkaLayer::create();
		addChild(game);

		auto keyboard = EventListenerKeyboard::create();
		keyboard->onKeyReleased = CC_CALLBACK_2(MatryoshkaScene::onKeyReleased, this);
		getEventDispatcher()->addEventListenerWithSceneGraphPriority(keyboard, this);

		return true;
	}

	void MatryoshkaScene::btnBackPressed(Ref*)
	{
		switchToMainMenu();
	}

	void MatryoshkaScene::showGameOver()
	{
		LastScore = game->score;
		Director::getInstance()->replaceScene(TransitionFade::create(1.0f, 
			ExtraGameOverScene::createScene(Games::Matryoshka)));

		TotalScore += LastScore;
		HighScore = max(LastScore, HighScore);
		SaveSettings();
	}

	void MatryoshkaScene::switchToMainMenu(float)
	{
		showGameOver();
	}

	void MatryoshkaScene::LoadSettings()
	{
		auto d = UserDefault::getInstance();
		LastScore = d->getIntegerForKey("MatryoshkaLastScore", LastScore);
		HighScore = d->getIntegerForKey("MatryoshkaHighScore", HighScore);
		TotalScore = d->getIntegerForKey("MatryoshkaTotalScore", TotalScore);
	}

	void MatryoshkaScene::SaveSettings()
	{
		auto d = UserDefault::getInstance();
		d->setIntegerForKey("MatryoshkaLastScore", LastScore);
		d->setIntegerForKey("MatryoshkaHighScore", HighScore);
		d->setIntegerForKey("MatryoshkaTotalScore", TotalScore);
		GameSettings::SaveSettings();
	}

	void MatryoshkaScene::onKeyReleased(EventKeyboard::KeyCode keyCode, Event* event)
	{
		if (keyCode == EventKeyboard::KeyCode::KEY_ESCAPE)
			btnBackPressed(nullptr);

		if (keyCode == Key_t::KEY_LEFT_ARROW)
			game->move(Directions::Left);
		else if (keyCode == Key_t::KEY_RIGHT_ARROW)
			game->move(Directions::Right);
		else if (keyCode == Key_t::KEY_UP_ARROW)
			game->move(Directions::Up);
		else if (keyCode == Key_t::KEY_DOWN_ARROW)
			game->move(Directions::Down);

		if (keyCode == Key_t::KEY_SPACE)
			Ooze::SendString("GetScores");
	}
}