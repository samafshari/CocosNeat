#include "MiniMainMenu.h"
#include "BackgroundWidget.h"
#include "BundleGamesScene.h"
#include "Widgets.h"
#include "Sidebar.h"
#include "Banner.h"
#include "ExtraGameOverScene.h"

#define TAG_LBLLASTSCORE 10
#define TAG_LBLHIGHSCORE 11

Scene* MiniMainMenu::createScene(Games game, Scene* previous, Layer* background)
{
    auto scene = Scene::create();
	auto layer = MiniMainMenu::create();
	layer->game = game;
	layer->previousScene = previous;
	if (previous) previous->retain();
	if (background) layer->addChild(background);
	else
	{
		auto bgcolor = LayerColor::create(GameInfos[game].color, DESIGN_X, DESIGN_Y);
		bgcolor->setPosition(Point::ZERO);
		layer->addChild(bgcolor);

#if !defined(LIGHT_THEME)
		layer->addChild(BackgroundWidget::createWithoutBackground());
		layer->addChild(Vignette::create());
#endif
	}

	layer->createGamePanel();
    scene->addChild(layer);
    return scene;
}

bool MiniMainMenu::init()
{
    if ( !Layer::init() )
    {
        return false;
    }
    
	auto keyboard = EventListenerKeyboard::create();
	keyboard->onKeyReleased = CC_CALLBACK_2(MiniMainMenu::onKeyReleased, this);
	getEventDispatcher()->addEventListenerWithSceneGraphPriority(keyboard, this);

	ExtraGameOverScene::End = CC_CALLBACK_0(MiniMainMenu::gameFinished, this);

    return true;
}

vector<string> MiniMainMenu::generateBannerStats()
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

void MiniMainMenu::gameFinished()
{
	scoresBanner->init(generateBannerStats());
	recursiveAction(scoresBanner, [](Node* node) { node->setColor(Color3B::BLACK); });
}

void MiniMainMenu::createGamePanel()
{
	log("MiniMainMenu creating for " + GameInfos[game].title);
	if (GameInfos[game].logo != "")
	{
		gameLogo = MAKESPRITE(GameInfos[game].logo);
		addChild(gameLogo);
		gameLogo->setAnchorPoint(Point(0.5f, 1.05f));
		gameLogo->setScale(0.85f);
		gameLogo->runAction(RepeatForever::create(Sequence::createWithTwoActions(
			ScaleTo::create(3.0f, 0.9f),
			ScaleTo::create(3.0f, 0.85f)
			)));
	}
	else
	{
		gameLogo = Node::create();
		addChild(gameLogo);
	}
		
	gameLogo->setPosition(VCP(0.5f, 0.99f));

	hbar = HBar::create();
	addChild(hbar);

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

	pMenu = Menu::create();
	pMenu->setPosition(Point::ZERO);
	addChild(pMenu, 1);

	btnBuy = MakeButton("btnGreenUp", "btnGreenDn", LS("Play"), 42.0f,
		CC_CALLBACK_1(MiniMainMenu::btnBuyPressed, this));
	btnBuy->setPosition(VCP(0.5f, 0.17f));
	pMenu->addChild(btnBuy);

	auto gameTitle = Label::createWithTTF(LS(GameInfos[game].title), BOLD_TTF, 60.0f);
	gameTitle->setHorizontalAlignment(TextHAlignment::CENTER);
	gameTitle->setPositionX(VPX(0.5f));
	gameTitle->setPositionY(gameLogo->getBoundingBox().getMinY());
	gameTitle->setAnchorPoint(Point(0.5f, 1.05f));
	addChild(gameTitle);

	auto rotY =
		btnBuy->getBoundingBox().getMaxY() +
		(gameTitle->getBoundingBox().getMinY() - btnBuy->getBoundingBox().getMaxY()) * 0.5f -
		PY(0.5f);
	rotParent = Node::create();
	rotParent->setPositionY(rotY);
	addChild(rotParent);

	rotator = BannerRotator::create(banners, true);
	rotParent->addChild(rotator, 2);
    
	btnFacebook = MakeButton("btnfacebookup", "btnfacebookdn", " ", 10, [](Ref*) {	OpenFacebookPage();	});
	hbar->addChild(btnFacebook);

	btnRate = MakeButton("Platformer_itemsup", "Platformer_itemsdn", " ", 10.0f, [](Ref*) { OpenStorePage(); });
	hbar->addChild2(btnRate);


#ifdef LIGHT_THEME
	gameTitle->setColor(Color3B::BLACK);
	recursiveAction(rotator, [](Node* node) { node->setColor(Color3B::BLACK); });
	recursiveAction(scoresBanner, [](Node* node) { node->setColor(Color3B::BLACK); });
#endif

#if (CC_TARGET_PLATFORM == CC_PLATFORM_IOS)
	float bottomH = hbar->getContentSize().height;
	auto btnHighUp = LayerColor::create(Color4B(50, 50, 50, 0), DESIGN_X * 0.5f, bottomH);
	auto btnHighDn = LayerColor::create(Color4B(50, 50, 80, 255), DESIGN_X * 0.5f, bottomH);
	auto btnHighUpT = Label::createWithTTF(LS("BTN_HIGHSCORES"), NORMAL_TTF, 34.0f);
	auto btnHighDnT = Label::createWithTTF(LS("BTN_HIGHSCORES"), NORMAL_TTF, 34.0f);
	btnHighUpT->setHorizontalAlignment(TextHAlignment::CENTER);
	btnHighDnT->setHorizontalAlignment(TextHAlignment::CENTER);
	btnHighUpT->setPosition(Point(DESIGN_X * 0.25f, bottomH * 0.5f));
	btnHighDnT->setPosition(Point(DESIGN_X * 0.25f, bottomH * 0.5f));
	btnHighUp->addChild(btnHighUpT);
	btnHighDn->addChild(btnHighDnT);
	auto btnHigh = MenuItemSprite::create(btnHighUp, btnHighDn, CC_CALLBACK_1(MiniMainMenu::btnLeaderboardsPressed, this));
	btnHigh->setAnchorPoint(Point(0.5f, 0.0f));
	btnHigh->setPosition(CP(0.5f, 0.0f));
	pMenu->addChild(btnHigh);
#else
	auto txtHighScore = Label::createWithTTF(LS("TXT_HIGHSCORE"), NORMAL_TTF, 34.0f);
	txtHighScore->setPositionX(PX(0.5f));
	txtHighScore->setPositionY(hbar->getContentSize().height * 0.5f);
	txtHighScore->setAnchorPoint(Point(0.5f, 0.0f));
	txtHighScore->setHorizontalAlignment(TextHAlignment::CENTER);
	addChild(txtHighScore);

	auto lblHighScore = Label::createWithBMFont(NORMAL_FNT, " ");
	lblHighScore->setPositionX(PX(0.5f));
	lblHighScore->setPositionY(hbar->getContentSize().height * 0.52f);
	lblHighScore->setAnchorPoint(Point(0.5f, 1.0f));
	lblHighScore->setHorizontalAlignment(TextHAlignment::CENTER);
	lblHighScore->setTag(TAG_LBLHIGHSCORE);
	addChild(lblHighScore);

	schedule(schedule_selector(MiniMainMenu::updateLabels), 1.0f);
	updateLabels(0);

#ifdef FA
	txtHighScore->setAnchorPoint(Point(0.5f, 0.1f));
	lblHighScore->setAnchorPoint(Point(0.5f, 0.9f));
#endif
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

void MiniMainMenu::updateLabels(float)
{
	static_cast<Label*>(getChildByTag(TAG_LBLHIGHSCORE))->setString(nToString(*GameInfos[game].pHighScore));
	//static_cast<Label*>(getChildByTag(TAG_LBLLASTSCORE))->setString("Last Score: " + nToString(*GameInfos[game].pLastScore));
}

void MiniMainMenu::onKeyReleased(EventKeyboard::KeyCode keyCode, Event* event)
{
	if (keyCode == EventKeyboard::KeyCode::KEY_ESCAPE)
		menuCloseCallback(this);
}

void MiniMainMenu::menuCloseCallback(Ref* pSender)
{
	if (previousScene)
		Director::getInstance()->replaceScene(TransitionCrossFade::create(0.5f, previousScene));
	else
	    Director::getInstance()->popScene();
}

void MiniMainMenu::btnBuyPressed(Ref*)
{
    BundleGamesScene::RunGame(game);
}

void MiniMainMenu::btnLeaderboardsPressed(Ref*){
#if IS_IOS
    GKHWrapperCpp gkh;
    gkh.showLeaderBoard();
#endif
}

void MiniMainMenu::btnAchievementsPressed(Ref*)
{
#if IS_IOS
    GKHWrapperCpp gkh;
    gkh.showAchievements();
#endif
}