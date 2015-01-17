#include "DronesScene.h"
#include "BackgroundWidget.h"
#include "DronesKeyboardController.h"
#include "DronesSwipeController.h"
#include "Ooze.h"
#include "Widgets.h"

using namespace Drones;

Scene* DronesScene::StartEggplantGame()
{
	//Create a new game
	GameSettings::DronesLastScore = 0;
	GameSettings::LastMoves = 0;
	GameSettings::LastTime = 60;
	GameSettings::LastKills = 0;
	GameSettings::LastWins = 0;
	GameSettings::CurrentLevelId = -1;
	GameSettings::DronesArcadePlayed++;
	GameSettings::SaveSettings();
	Ooze::InstantSend({ { "Event", "GameStart" }, { "Game", "DronesEggplant" } });
	GameSettings::OozeGameStats();
	return DronesScene::eggplantScene();
}

Scene* DronesScene::StartRandomGame()
{
    //Create a new game
	GameSettings::DronesLastScore = 0;
	GameSettings::LastMoves = 0;
	GameSettings::LastTime = 60;
	GameSettings::LastKills = 0;
	GameSettings::LastWins = 0;
	GameSettings::CurrentLevelId = -1;
    GameSettings::DronesArcadePlayed ++;
    GameSettings::SaveSettings();
    Ooze::InstantSend({{"Event", "GameStart"}, {"Game", "DronesArcade"}});
    GameSettings::OozeGameStats();
	return DronesScene::randomScene();
}

Scene* DronesScene::scene()
{
	Scene *scene = Scene::create();
	DronesScene *layer = DronesScene::create();
	layer->gameType = GameTypes::Level;
	layer->levelData = ReadLinesFromFile(GameSettings::GetLevelFilename());
	layer->addGame();
	layer->createNormalGame();
	scene->addChild(layer);
	return scene;
}

Scene* DronesScene::randomScene()
{
	Scene *scene = Scene::create();
	DronesScene *layer = DronesScene::create();
	layer->gameType = GameTypes::Random;
	{
		auto meta = GAME_OF_DRONES_MAPMETA;
		auto levelRaw = MapGenerator::Generate(meta);
		//if (GameSettings::LastWins % 2 == 0)
		//	MapGenerator::AddItem('g', levelRaw, 4, meta.width, meta.height);
		auto level = MapGenerator::Flatten(levelRaw);
		level.push_back("noscroll");
		level.push_back("scale 0.7");
		layer->levelData = level;
	}
	layer->addGame();

	layer->alignCamera();
	layer->createArcadeGame();
	scene->addChild(layer);
	return scene;
}

Scene* DronesScene::eggplantScene()
{
	Scene *scene = Scene::create();
	DronesScene *layer = DronesScene::create();
	layer->gameType = GameTypes::Eggplant;
	{
		auto meta = EGGPLANT_MAPMETA;
		auto levelRaw = MapGenerator::Generate(meta);
		for (
			int robotCount = min(5, GameSettings::LastWins); 
			robotCount >= 0; 
			robotCount--)
			MapGenerator::AddItem('m', levelRaw, 3, meta.width, meta.height);
		auto level = MapGenerator::Flatten(levelRaw);
		level.push_back("noscroll");
		level.push_back("scale 0.7");
		layer->levelData = level;
	}
	layer->addGame();

	layer->alignCamera();
	layer->createArcadeGame();
	layer->schedule(schedule_selector(DronesScene::updateRandomTimer), 1.0f);
	scene->addChild(layer);
	return scene;
}

void DronesScene::addGame()
{
    game = DronesLayer::create(this, levelData);
	gameParent->addChild(game);
	alignCamera();
}

bool DronesScene::init()
{
	dead = false;
	over = false;
	scrollable = false;
	moved = false;

	if (!Layer::init()) return false;

	Size visibleSize = Director::getInstance()->getVisibleSize();

	bgParent = Node::create();
	addChild(bgParent);

#if !defined(BURGERMAN)
	addChild(BackgroundWidget::createWithoutBackground());
#endif

	addChild(Vignette::create());

	auto hooker = TopAnchor::create();
    addChild(hooker, 1);
	gameParent = Node::create();
	//hooker->
    addChild(gameParent);
	gameParent->setTag(GAMEPARENTTAG);

	menuY = 0.94f;
    
	pMenu = HBar::create();
	pMenu->moveToTop();

	//pMenu->setPosition(VCP(0.9f, menuY));
	hooker->addChild(pMenu, 1);
	//createNormalGame();
	//createArcadeGame();
	addInput();
	editorInit();
	return true;
}

void DronesScene::addBackground()
{
	Size visibleSize = Director::getInstance()->getVisibleSize();
	Node* bgcolor;
#if !defined(BURGERMAN)
	if (gameType == GameTypes::Level)
		bgcolor = MAKESPRITE("bg" + nToString(GameSettings::CurrentEpisode));
	else
		bgcolor = MAKESPRITE("bg");

	bgcolor->setPosition(visibleSize.width * 0.5f, visibleSize.height * 0.5f);
	bgcolor->setAnchorPoint(Point(0.5f, 0.5f));

	bgcolor->setColor(Color3B(20, 20, 80));
#else
	if (gameType == GameTypes::Level)
		bgcolor = BackgroundWidget::createWithDefaultBackground(Color3B::WHITE, true, GameSettings::CurrentEpisode + 1);
	else
		bgcolor = BackgroundWidget::createWithDefaultBackground(Color3B::WHITE);
#endif

	bgParent->addChild(bgcolor);
}

void DronesScene::createArcadeGame()
{
	addBackground();

	auto bottomHeight = PY(0.13f);
	
	auto currentStats = createRandomStats(this, PX(1.0f), bottomHeight, Color3B(255, 255, 255), Color4B(0, 0, 0, 0));
	currentStats->setTag(NEWSTATSTAG);
	//currentStats->setPositionY(-bottomHeight);
	for (auto node : currentStats->getChildren())
		static_cast<Label*>(node)->enableShadow(Color4B::BLACK, Size(0.5f, -1.0f), 1);

	float btnX = 0.00f;
    
#if (CC_TARGET_PLATFORM != CC_PLATFORM_ANDROID)
	auto btnBack = MAKEBUTTON("btnExitUp", "btnExitDn", DronesScene::btnSurrenderPressed);
	pMenu->addChild(btnBack);
#endif

	if (gameType == GameTypes::Random)
	{
		auto btnSkip = MAKEBUTTON("btnNextUp", "btnNextDn", DronesScene::btnSkipLevel);
		pMenu->addChild2(btnSkip);
	}

	pMenu->addChild(SoundButton::create());
	pMenu->addChild(MusicButton::create());
	
#ifndef FA
	auto btnHelp = MAKEBUTTON("btnHelpUp", "btnHelpDn", DronesScene::btnHelpPressed);
	if (gameType == GameTypes::Random) pMenu->addChild(btnHelp);
	else pMenu->addChild2(btnHelp);

	//game->schedule(schedule_selector(DronesLayer::spawnBonus), 10);
#endif
}

void DronesScene::createNormalGame()
{
	addBackground();

	auto bottomHeight = PY(0.15f);

	auto data = UserDefault::getInstance();
	if (data->getIntegerForKey(("wone" + nToString(GameSettings::CurrentEpisode) + "l" + nToString(GameSettings::CurrentLevelId)).c_str(), -1) != TAG_WIN)
	{
		//no records for this level
		Layer* bottomLayer = Layer::create();
		bottomLayer->setTag(OLDSTATSTAG);
		addChild(bottomLayer);
	}
	else
	{
		auto bottomLayer = createGameStats(this, PX(1.0f), bottomHeight, Color3B::ORANGE, Color4B(0, 0, 0, 0));
		bottomLayer->setTag(OLDSTATSTAG);
		for (auto node : bottomLayer->getChildren())
			static_cast<Label*>(node)->enableShadow(Color4B::BLACK, Size(0.5f, -1.0f), 1);
	}

	auto currentStats = createGameStats(this, PX(1.0f), bottomHeight, Color3B(255, 255, 255), Color4B(0, 0, 0, 0));
	currentStats->setTag(NEWSTATSTAG);
	currentStats->setPositionY(-bottomHeight);
	for (auto node : currentStats->getChildren())
		static_cast<Label*>(node)->enableShadow(Color4B::BLACK, Size(0.5f, -1.0f), 1);

	//getChildByTag(GAMEPARENTTAG)->runAction(MoveTo::create(0.01f, Point(0, -1.5f * game->blockSize * BLOCK_SCALE)));

    auto hooker = TopAnchor::create();
    addChild(hooker, 1);
    
	auto menuChooser = Menu::create();
	menuChooser->setPosition(Point::ZERO);
	hooker->addChild(menuChooser, 1);

	float btnX = 0.00f;
	menuY = pMenu->getBoundingBox().getMidY();// pMenu->getContentSize().height * 0.5f + pMenu->getPositionY();

	auto btnPrevLevel = MAKEBUTTON("btnCoverUp", "btnCoverDn", DronesScene::btnPrevLevel);
	btnPrevLevel->setScale(buttonScale);
	btnPrevLevel->setPositionX(VPX(0.05f));
	btnPrevLevel->setPositionY(menuY);
	if (GameSettings::CurrentLevelId > 0)
		menuChooser->addChild(btnPrevLevel);

	auto btnNextLevel = MAKEBUTTON("btnCoverUp", "btnCoverDn", DronesScene::btnNextLevel);
	btnNextLevel->setScale(buttonScale);
	btnNextLevel->setRotation(180);
	btnNextLevel->setPositionX(VPX(0.3f));
	btnNextLevel->setPositionY(menuY);
	if (GameSettings::CurrentLevelId < GameSettings::GetUnlockedLevelId() &&
		GameSettings::CurrentLevelId < GameSettings::GetMaximumLevelId() - 1)
		menuChooser->addChild(btnNextLevel);

	auto btnLevelLbl = Label::createWithBMFont(NORMAL_FNT, nToString(1 + GameSettings::CurrentLevelId));
	btnLevelLbl->setAnchorPoint(Point(0.5f, 0.5f));
	btnLevelLbl->setHorizontalAlignment(TextHAlignment::CENTER);
	auto btnLevelNo = MenuItemLabel::create(btnLevelLbl, [&](Ref *sender){DronesScene::btnChooseLevel(sender); });
	btnLevelNo->setPositionX(VPX(0.175f));
	btnLevelNo->setPositionY(menuY);
	menuChooser->addChild(btnLevelNo);
	
#if (CC_TARGET_PLATFORM != CC_PLATFORM_ANDROID)
	auto btnBack = MAKEBUTTON("btnExitUp", "btnExitDn", DronesScene::btnBackPressed);
	pMenu->addChild2(btnBack);
#endif

	pMenu->addChild2(SoundButton::create());
	pMenu->addChild2(MusicButton::create());

#ifndef FA
	auto btnHelp = MAKEBUTTON("btnHelpUp", "btnHelpDn", DronesScene::btnHelpPressed);
	pMenu->addChild2(btnHelp);
#endif

	auto btnRestart = MAKEBUTTON("btnRefUp", "btnRefDn", DronesScene::btnRefreshPressed);
	pMenu->addChild2(btnRestart);

}

void DronesScene::addInput()
{
	auto keyboard = EventListenerKeyboard::create();
	keyboard->onKeyReleased = CC_CALLBACK_2(DronesScene::onKeyReleased, this);
	getEventDispatcher()->addEventListenerWithSceneGraphPriority(keyboard, this);

	InputController* controller = SwipeController::create();
	controller->setTag(2001);

	inputs.push_back(KeyboardController::create());
	inputs.push_back(controller);

	for (auto input : inputs)
	{
		addChild(input, 10);
		input->reset();
	}
}