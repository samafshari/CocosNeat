#include "BlobMenu.h"
#include "BlobScene.h"
#include "BackgroundWidget.h"
#include "BundleGamesScene.h"
#include "Blob.h"
#include "Widgets.h"
#include "Sidebar.h"
#include "Banner.h"
#include "ExtraGameOverScene.h"

namespace Blob
{
	const int TAG_LBLLASTSCORE = 10;
	const int TAG_LBLHIGHSCORE = 11;

	Scene* BlobMenu::createScene(Games game)
	{
		auto scene = Scene::create();
		auto layer = BlobMenu::create();
		layer->game = game;
		layer->createGamePanel();
		scene->addChild(layer);
		return scene;
	}

	bool BlobMenu::init()
	{
		if (!Layer::init())
		{
			return false;
		}

		auto keyboard = EventListenerKeyboard::create();
		keyboard->onKeyReleased = CC_CALLBACK_2(BlobMenu::onKeyReleased, this);
		getEventDispatcher()->addEventListenerWithSceneGraphPriority(keyboard, this);

		ExtraGameOverScene::End = CC_CALLBACK_0(BlobMenu::gameFinished, this);

		GameSettings::PlayMusic(SOUND_MENU_MUSIC);
		return true;
	}

	vector<string> BlobMenu::generateBannerStats()
	{
		vector<string> bdata{
			string_format(LS("TXT_HIGHSCORECOLON"), *GameInfos[game].pHighScore),
			string_format(LS("TXT_LASTSCORECOLON"), *GameInfos[game].pLastScore) };

		log(string_format("GlobalRanking: %d", GameSettings::GlobalRanking));
		log(string_format("GlobalLevelupDelta: %d", GameSettings::GlobalLevelupDelta));

		if (GameSettings::GlobalRanking > 0)
		{
			bdata.push_back(string_format(LS("TXT_GLOBALRANKING"),
				GameSettings::GlobalRanking, GameSettings::GlobalTotalScore));
			if (GameSettings::GlobalLevelupDelta == -1 || GameSettings::GlobalRanking == 1)
			{

				bdata.push_back(LS("TXT_WORLDCHAMPION"));
			}
			else if (GameSettings::GlobalLevelupDelta > 0)
			{
				bdata.push_back(
					string_format(LS("TXT_LEVELUPSCORE1"), GameSettings::GlobalLevelupDelta));
				bdata.push_back(LS("TXT_LEVELUPSCORE2"));
			}
		}
		else
		{
			bdata.push_back(LS("TXT_CONNECT1"));
			bdata.push_back(LS("TXT_CONNECT2"));
		}
		return bdata;
	}

	void BlobMenu::gameFinished()
	{
		GameSettings::PlayMusic(SOUND_MENU_MUSIC);
		scoresBanner->init(generateBannerStats());
	}

	void BlobMenu::createGamePanel()
	{
		Size visibleSize = Director::getInstance()->getVisibleSize();
		auto bgcolor = LayerColor::create(GameInfos[game].color, visibleSize.width, visibleSize.height);
		bgcolor->setPosition(Point::ZERO);
		addChild(bgcolor);

		addChild(BlobRainbowBackground::create());
		addChild(LayerColor::create(Color4B(0, 0, 0, 100), DESIGN_X, DESIGN_Y));
		addChild(Vignette::create());

		auto gameTitle = Label::createWithTTF(LS("Blob"), NORMAL_TTF, 90.0f);
		gameTitle->setHorizontalAlignment(TextHAlignment::CENTER);
		gameTitle->setPosition(VCP(0.5f, 0.85f));
		gameTitle->setAnchorPoint(Point(0.5f, 0.5f));
		
		addChild(gameTitle);

		auto hooker = TopAnchor::create();
		addChild(hooker, 1);

		auto HBar = HBar::create();
		addChild(HBar);

		HBar->addChild(MusicButton::create());
		//HBar->addChild2(SoundButton::create());

		vector<Banner*> banners;
		//#if IS_IOS
		auto highScore = HighScoreBanner::create();
		highScore->chance = 2;
		GameSettings::DefaultLeaderBoard = GameInfos[game].leaderboardId
#if defined(FA)
			+"FA"
#endif
			;
		highScore->categoryToShow = GameSettings::DefaultLeaderBoard;
		banners.push_back(highScore);
		//#endif

		banners.push_back(scoresBanner = Banner::create(generateBannerStats()));
		banners.push_back(Banner::create({LS(GameInfos[game].description)}));

#if FA
		banners.push_back(Banner::create({ LS("DESC_BLOB") }));
#else
		banners.push_back(Banner::create({
			LS("TXT_HELP_BLOB1"),
			"",
			LS("TXT_HELP_BLOB2"),
			LS("TXT_HELP_BLOB3"),
			LS("TXT_HELP_BLOB4"),
			LS("TXT_HELP_BLOB5")
		}));
		banners.push_back(Banner::create({ LS("TXT_SOURCE") }));
#endif

		auto rotParent = Node::create();
		addChild(rotParent);

#if IS_IOS
		rotParent->setPositionY(PY(0.05f));
		gameTitle->setPositionY(gameTitle->getPositionY() + PY(0.05f));
#endif

		auto rotator = BannerRotator::create(banners, true);
		rotParent->addChild(rotator, 2);

		auto pMenu = Menu::create();
		pMenu->setPosition(Point::ZERO);
		addChild(pMenu, 1);
		
		auto btnFacebook = MakeButton("btnfacebookup", "btnfacebookdn", " ", 10, [](Ref*) {	OpenFacebookPage();	});
		HBar->addChild(btnFacebook);

		auto btnBuy = MakeButton("btnGreenUp", "btnGreenDn", LS("Play"), 42.0f,
			CC_CALLBACK_1(BlobMenu::btnBuyPressed, this));
		btnBuy->setPosition(VCP(0.5f, 0.19f));
		pMenu->addChild(btnBuy);
		float y = btnBuy->getPositionY();
		float deltaY = btnBuy->getBoundingBox().size.height * 1.1f;

		auto btnRate = MakeButton("Platformer_itemsup", "Platformer_itemsdn", " ", 10.0f, [](Ref*) { OpenStorePage(); });
		HBar->addChild2(btnRate);

		auto btnCopy = MakeButton("btncopyrightup", "btncopyrightdn", " ", 10, [](Ref*) {
			Director::getInstance()->replaceScene(TransitionSlideInB::create(1.0f, BlobCredits::createScene()));
		});
		HBar->addChild2(btnCopy);

#if (CC_TARGET_PLATFORM == CC_PLATFORM_IOS)
		auto btnScores = MakeButton("btnUp", "btnDn", LS("BTN_HIGHSCORES"), 42.0f, CC_CALLBACK_1(BlobMenu::btnLeaderboardsPressed, this));
		btnScores->setPosition(Point(PX(0.5f), y += deltaY));
        btnScores->setScale(0.75f);
		pMenu->addChild(btnScores);
#endif

		auto txtHighScore = Label::createWithTTF(LS("TXT_HIGHSCORE"), NORMAL_TTF, 34.0f);
		txtHighScore->setPositionX(PX(0.5f));
		txtHighScore->setPositionY(HBar->getContentSize().height * 0.5f);
		txtHighScore->setAnchorPoint(Point(0.5f, 0.0f));
		txtHighScore->setHorizontalAlignment(TextHAlignment::CENTER);
		addChild(txtHighScore);

		auto lblHighScore = Label::createWithBMFont(NORMAL_FNT, " ");
		lblHighScore->setPositionX(PX(0.5f));
		lblHighScore->setPositionY(HBar->getContentSize().height * 0.5f);
		lblHighScore->setAnchorPoint(Point(0.5f, 1.01f));
		lblHighScore->setHorizontalAlignment(TextHAlignment::CENTER);
		lblHighScore->setTag(TAG_LBLHIGHSCORE);
		addChild(lblHighScore);

		schedule(schedule_selector(BlobMenu::updateLabels), 1.0f);
		updateLabels(0);

#ifdef FA
		txtHighScore->setAnchorPoint(Point(0.5f, 0.1f));
		lblHighScore->setAnchorPoint(Point(0.5f, 0.9f));
#endif
		
#if (CC_TARGET_PLATFORM != CC_PLATFORM_ANDROID) && !defined(MINI_GAME)
		auto btnMenu = MAKEBUTTON("btnMenuUp", "btnMenuDn",
			[](Ref*){
			Director::getInstance()->popScene();
		});
		btnMenu->setPosition(VCP(0, 1));
		btnMenu->setAnchorPoint(Point(-0.5, 1.5));
		btnMenu->setScale(1.3f);
		pMenu->addChild(btnMenu);
#endif
	}

	void BlobMenu::updateLabels(float)
	{
		static_cast<Label*>(getChildByTag(TAG_LBLHIGHSCORE))->setString(nToString(*GameInfos[game].pHighScore));
		//static_cast<Label*>(getChildByTag(TAG_LBLLASTSCORE))->setString("Last Score: " + nToString(*GameInfos[game].pLastScore));
	}

	void BlobMenu::onKeyReleased(EventKeyboard::KeyCode keyCode, Event* event)
	{
		if (keyCode == EventKeyboard::KeyCode::KEY_ESCAPE)
			menuCloseCallback(this);
	}

	void BlobMenu::menuCloseCallback(Ref* pSender)
	{
		Director::getInstance()->end();
	}

	void BlobMenu::btnBuyPressed(Ref*)
	{
		GameSettings::StopMusic();
		BundleGamesScene::RunGame(game);
	}

	void BlobMenu::btnLeaderboardsPressed(Ref*){
#if IS_IOS
		GKHWrapperCpp gkh;
		gkh.showLeaderBoard();
#endif
	}

	void BlobMenu::btnAchievementsPressed(Ref*)
	{
#if IS_IOS
		GKHWrapperCpp gkh;
		gkh.showAchievements();
#endif
	}


	///////////////////// BLOBCREDITS
	Scene* BlobCredits::createScene()
	{
		auto scene = Scene::create();
		auto layer = BlobCredits::create();
		scene->addChild(layer);
		return scene;
	}

	bool BlobCredits::init()
	{
		if (!Layer::init()) return false;

		addChild(LayerColor::create(Color4B(10, 10, 10, 255), DESIGN_X, DESIGN_Y));

		text = Label::createWithBMFont(
			FIXED_FNT_ENGLISH,
			FileUtils::getInstance()->getStringFromFile("licenses.txt"));

		text->setAnchorPoint(Point(0.5f, 1.0f));
		text->setPosition(CP(0.5f, 1.0f));
		text->setScale(0.5f);
		text->setColor(Color3B::WHITE);
		text->setHorizontalAlignment(TextHAlignment::CENTER);
		text->setVerticalAlignment(TextVAlignment::TOP);
		addChild(text);


#if (CC_TARGET_PLATFORM != CC_PLATFORM_ANDROID)
		auto menu = Menu::create();
		menu->setPosition(Point::ZERO);
		addChild(menu);

		auto btnBackUp = LayerColor::create(Color4B(100, 5, 5, 255), DESIGN_X, PY(0.08f));
		auto btnBackUpSprite = MAKESPRITE("btnDownArrowUp");
		btnBackUpSprite->setPosition(CP(0.5f, 0.04f));
		btnBackUp->addChild(btnBackUpSprite);

		auto btnBackDn = LayerColor::create(Color4B(50, 5, 5, 255), DESIGN_X, PY(0.08f));
		auto btnBackDnSprite = MAKESPRITE("btnDownArrowUp");
		btnBackDnSprite->setPosition(CP(0.5f, 0.04f));
		btnBackDn->addChild(btnBackDnSprite);

		auto btnBack = MenuItemSprite::create(btnBackUp, btnBackDn, CC_CALLBACK_1(BlobCredits::menuCloseCallback, this));
		btnBack->setAnchorPoint(Point::ZERO);
		menu->addChild(btnBack);
#endif

		auto keyboard = EventListenerKeyboard::create();
		keyboard->onKeyReleased = CC_CALLBACK_2(BlobCredits::onKeyReleased, this);
		getEventDispatcher()->addEventListenerWithSceneGraphPriority(keyboard, this);
		
		auto touchListener = EventListenerTouchOneByOne::create();
		touchListener->onTouchBegan = CC_CALLBACK_2(BlobCredits::onTouchBegan, this);
		touchListener->onTouchMoved = CC_CALLBACK_2(BlobCredits::onTouchMoved, this);
		getEventDispatcher()->addEventListenerWithSceneGraphPriority(touchListener, this);

		return true;
	}

	void BlobCredits::menuCloseCallback(Ref*)
	{
		text->runAction(MoveTo::create(0.3f, CP(0.5f, 1.0f)));
		Director::getInstance()->replaceScene(
			TransitionSlideInT::create(1.0f, BlobMenu::createScene(APPLICATION)));
	}

	void BlobCredits::onKeyReleased(EventKeyboard::KeyCode keyCode, Event* event)
	{
		if (keyCode == EventKeyboard::KeyCode::KEY_ESCAPE)
			menuCloseCallback(nullptr);
	}

	bool BlobCredits::onTouchBegan(Touch*, Event*)
	{
		return true;
	}

	void BlobCredits::onTouchMoved(Touch* t, Event*)
	{
		text->setPositionY( text->getPositionY() + t->getDelta().y );
		auto aabb = text->getBoundingBox();
		if (text->getPositionY() < VPY(1.0f)) text->setPositionY(VPY(1.0f));
		if (aabb.getMinY() > VPY(0.1f)) text->setPositionY(VPY(0.1f) + aabb.size.height);
	}
}