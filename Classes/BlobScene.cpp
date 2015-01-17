#include "ExtraGameOverScene.h"
#include "BlobScene.h"
#include "BlobLayer.h"
#include "Ooze.h"
#include "Sidebar.h"
#include "Widgets.h"
#include "Blob.h"

namespace Blob
{
	int BlobScene::LastScore = 0;
	int BlobScene::HighScore = 0;
	int BlobScene::TotalScore = 0;

	Scene* BlobScene::scene()
	{
		Ooze::InstantSend({ { "Event", "GameStart" }, { "Game", "Blob" } });
		Scene *scene = Scene::create();
		BlobScene *layer = BlobScene::create();
		scene->addChild(layer);
		return scene;
	}

	bool BlobScene::init()
	{
		if (!Layer::init()) return false;

		LastScore = 0;
		
		Size visibleSize = Director::getInstance()->getVisibleSize();
		
		background = BlobRainbowBackground::create();
		addChild(background);

		game = BlobLayer::create();
		addChild(game);

		auto HBar = HBar::create();
		addChild(HBar);
		game->setPositionY(HBar->getContentSize().height);
		
#if (CC_TARGET_PLATFORM != CC_PLATFORM_ANDROID)
		auto btnBack = MAKEBUTTON("btnMenuUp", "btnMenuDn", btnBackPressed);
		HBar->addChild(btnBack);
#endif
		HBar->addChild(MusicButton::create());
		HBar->addChild2(SoundButton::create());

		auto btnPause = MAKEBUTTON("Platformer_pauseup", "Platformer_pausedn", btnPausedPressed);
		HBar->addChild2(btnPause);

		auto keyboard = EventListenerKeyboard::create();
		keyboard->onKeyReleased = CC_CALLBACK_2(BlobScene::onKeyReleased, this);
		getEventDispatcher()->addEventListenerWithSceneGraphPriority(keyboard, this);

		auto listener = EventListenerAcceleration::create(CC_CALLBACK_2(BlobScene::onAcceleration, this));
		getEventDispatcher()->addEventListenerWithSceneGraphPriority(listener, this);
		schedule(schedule_selector(BlobScene::update), 0, kRepeatForever, 1.0f);

		auto touchListener = EventListenerTouchOneByOne::create();
		touchListener->onTouchBegan = CC_CALLBACK_2(BlobScene::onTouchBegan, this);
		getEventDispatcher()->addEventListenerWithSceneGraphPriority(touchListener, this);

		scoreLabel = Label::createWithTTF("", NORMAL_TTF, 38.0f);
		scoreLabel->setColor(Color3B::WHITE);
		scoreLabel->setHorizontalAlignment(TextHAlignment::CENTER);
		scoreLabel->setPosition(Point(PX(0.5f), HBar->getContentSize().height * 0.5f));
		addChild(scoreLabel);
		
		pauseLayer = nullptr;

		giveScore(0);

		GameSettings::PlayMusic(SOUND_GAME_MUSIC);
		return true;
	}

	bool BlobScene::onTouchBegan(Touch*, Event*)
	{
		if (pauseLayer) btnPausedPressed(nullptr);
		return false;
	}

	void BlobScene::btnPausedPressed(Ref*)
	{
		if (pauseLayer)
		{
			removeChild(pauseLayer);
			pauseLayer = nullptr;
			recursiveResume(game);
		}
		else
		{
			pauseLayer = PauseLayer::create();
			addChild(pauseLayer);
			recursivePause(game);
		}
	}

	void BlobScene::giveScore(int d)
	{
		LastScore += d;
		scoreLabel->setString(nToString(LastScore) + " " +
			(LastScore == 1 ? LS("PointSmall") : LS("PointsSmall")));
		scoreLabel->runAction(Sequence::createWithTwoActions(
			ScaleTo::create(0.3f, 1.1f),
			ScaleTo::create(0.3f, 1.0f)
			));
	}

	void BlobScene::btnBackPressed(Ref*)
	{
		switchToMainMenu();
	}

	void BlobScene::showGameOver()
	{
		SimpleAudioEngine::getInstance()->stopBackgroundMusic();

		auto n = BlobRainbowBackground::create();
		n->addChild(LayerColor::create(Color4B(0, 0, 0, 100), DESIGN_X, DESIGN_Y));
		n->addChild(Vignette::create());

		Director::getInstance()->replaceScene(TransitionFade::create(1.0f, 
			ExtraGameOverScene::createScene(Games::Blob, n)));
		TotalScore += LastScore;
		HighScore = max(LastScore, HighScore);
		SaveSettings();
	}

	void BlobScene::switchToMainMenu(float)
	{
		showGameOver();
	}

	void BlobScene::LoadSettings()
	{
		auto d = UserDefault::getInstance();
		LastScore = d->getIntegerForKey("BlobLastScore", LastScore);
		HighScore = d->getIntegerForKey("BlobHighScore", HighScore);
		TotalScore = d->getIntegerForKey("BlobTotalScore", TotalScore);
	}

	void BlobScene::SaveSettings()
	{
		auto d = UserDefault::getInstance();
		d->setIntegerForKey("BlobLastScore", LastScore);
		d->setIntegerForKey("BlobHighScore", HighScore);
		d->setIntegerForKey("BlobTotalScore", TotalScore);
		GameSettings::SaveSettings();
	}

	void BlobScene::onKeyReleased(EventKeyboard::KeyCode keyCode, Event* event)
	{
		if (keyCode == EventKeyboard::KeyCode::KEY_ESCAPE)
			btnBackPressed(nullptr);
	}

	/////////////// PauseLayer
	bool PauseLayer::init()
	{
		if (!Layer::init()) return false;

		addChild(LayerColor::create(Color4B(0, 0, 0, 100), DESIGN_X, DESIGN_Y));
		addChild(Vignette::create());

		auto leftbar = LayerColor::create(Color4B::WHITE, PX(0.1f), PY(0.18f));
		leftbar->ignoreAnchorPointForPosition(false);
		leftbar->setAnchorPoint(Point(1.25f, -0.1f));
		leftbar->setPosition(CP(0.5f, 0.5f));
		addChild(leftbar);

		auto rightbar = LayerColor::create(Color4B::WHITE, PX(0.1f), PY(0.18f));
		rightbar->ignoreAnchorPointForPosition(false);
		rightbar->setAnchorPoint(Point(-0.25f, -0.1f));
		rightbar->setPosition(CP(0.5f, 0.5f));
		addChild(rightbar);

		auto label = Label::createWithTTF(LS("Paused"), NORMAL_TTF, 64.0f);
		label->setAnchorPoint(Point(0.5f, 1.5f));
		label->setPosition(CP(0.5f, 0.5f));
		addChild(label);

		return true;
	}

	/////////////// BlobRainbowBackground

	bool BlobRainbowBackground::init()
	{
		if (!Layer::init()) return false;
		
		auto bg = LayerGradient::create(Color4B(0, 133, 255, 255), Color4B(200, 200, 200, 255), Vec2(0, 1));
		addChild(bg);

		auto rainbow = Sprite::create("rainbow.png");
		rainbow->setPosition(CP(0.5f, 0.7f));
		addChild(rainbow);
        if (DESIGN_Y / DESIGN_X - 1024.0f / 768.0f < 0.01f) rainbow->setScale(1.25f, 1.25f);
        
#if (CC_TARGET_PLATFORM != CC_PLATFORM_IOS)
		ParticleSystemQuad* p = ParticleSun::create();
		p->setPositionType(tPositionType::RELATIVE);
		p->setPosition(CP(0.0f, 1.0f));
		p->setScale(10);
		addChild(p);
		p->runAction(FadeIn::create(1.0f));
#endif
        
		cloud1 = MAKESPRITE("cloud1");
		cloud1->setAnchorPoint(Point(1.1f, 0.5f));
		cloud1->setPositionX(0);
		cloud1->setPositionY(RANDF2(PY(0.5f), PY(1.0f)));
		addChild(cloud1);

		auto woods = Sprite::create("woods.png");
		woods->setPosition(CP(0.5f, 0.0f));
		woods->setAnchorPoint(Point(0.5f, 0.0f));
		woods->setFlippedX(true);
		addChild(woods);

		cloud2 = MAKESPRITE("cloud2");
		cloud2->setAnchorPoint(Point(-0.1f, 0.5f));
		cloud2->setPositionX(PX(1.0f));
		cloud2->setPositionY(RANDF2(PY(0.5f), PY(1.0f)));
		addChild(cloud2);

		schedule(schedule_selector(BlobRainbowBackground::moveCloud1));
		schedule(schedule_selector(BlobRainbowBackground::moveCloud2));
		return true;
	}

	void BlobRainbowBackground::moveCloud1(float dt)
	{
		cloud1->setPositionX(cloud1->getPositionX() + PX(0.01f) * dt);
		if (cloud1->getPositionX() > PX(2.0f))
		{
			cloud1->setPositionX(0);
			cloud1->setPositionY(RANDF2(PY(0.5f), PY(1.0f)));
			cloud1->setScale(RANDF2(1.0f, 2.0f));
		}
	}

	void BlobRainbowBackground::moveCloud2(float dt)
	{
		cloud2->setPositionX(cloud2->getPositionX() - PX(0.02f) * dt);
		if (cloud2->getPositionX() < -PX(0.4f))
		{
			cloud2->setPositionX(PX(1.0f));
			cloud2->setPositionY(RANDF2(PY(0.5f), PY(1.0f)));
			cloud2->setScale(RANDF2(1.0f, 2.0f));
		}
	}
}



