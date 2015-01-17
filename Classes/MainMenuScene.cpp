#include "helpers.h"
#include "MainMenuScene.h"
#include "CreditsScene.h"
#include "ChooserScene.h"
#include "HelpScene.h"
#include "BackgroundWidget.h"
#include "EpisodeChooserScene.h"
#include "BundleStoreScene.h"
#include "BundleGamesScene.h"
#include "Widgets.h"

Scene* MainMenu::scene()
{
    Scene *scene = Scene::create();
    MainMenu *layer = MainMenu::create();
    scene->addChild(layer);
    return scene;
}

static MenuItemSprite *btnSound;

bool MainMenu::init()
{
    if ( !Layer::init() )
    {
        return false;
    }
    
	Size visibleSize = Director::getInstance()->getVisibleSize();

	addChild(BackgroundWidget::createWithDefaultBackground(Color3B(20, 20, 100)));

	addChild(Vignette::create());
	
	auto hole = MAKESPRITE("blackholebig");
	addChild(hole);
	hole->setPosition(CP(0.5f, 0.7f));
	hole->runAction(RepeatForever::create(RotateBy::create(40.0, 365)));
	
	auto gamelogo = MAKESPRITE("gamelogo");
	addChild(gamelogo);
	gamelogo->setPosition(CP(0.5f, 0.7f));
	gamelogo->runAction(RepeatForever::create(Sequence::createWithTwoActions(
		ScaleTo::create(3.0f, 1.05f),
		ScaleTo::create(3.0f, 1.0f)
		)));
	/*
	auto gamelogotext = MAKESPRITE("gamelogotext");
	addChild(gamelogotext);
	gamelogotext->setPosition(gamelogo->getPosition());
	*/
	/////////// MENU
	auto pMenu = Menu::create(); 
	pMenu->setPosition(Point::ZERO);
	addChild(pMenu, 1);

	float fontSize = 42.0f;

	auto btnNewGame = MakeButton("btnUp", "btnDn", LS("StartGame"), fontSize, CC_CALLBACK_1(MainMenu::btnNewGamePressed, this));
	btnNewGame->setPosition(CP(0.5f, 0.4f));
	pMenu->addChild(btnNewGame);
    
#if IS_IOS
	auto btnHelp = MakeButton("btnUp", "btnDn", LS("GemStore"), fontSize, CC_CALLBACK_1(MainMenu::btnHelpPressed, this));
	btnHelp->setPosition(CP(0.5f, 0.3f));
	pMenu->addChild(btnHelp);

	auto btnScores = MakeButton("btnUp", "btnDn", LS("HighScores"), fontSize, CC_CALLBACK_1(MainMenu::btnScoresPressed, this));
	btnScores->setPosition(CP(0.5f, 0.2f));
	pMenu->addChild(btnScores);

	auto btnAchievements = MakeButton("btnUp", "btnDn", LS("Achievements"), fontSize, CC_CALLBACK_1(MainMenu::btnAchievementsPressed, this));
	btnAchievements->setPosition(CP(0.5f, 0.1f));
	pMenu->addChild(btnAchievements);
#else
    auto btnExtras = MakeButton("btnUp", "btnDn", LS("CoolStuff"), fontSize, CC_CALLBACK_1(MainMenu::btnExtrasPressed, this));
	btnExtras->setPosition(CP(0.5f, 0.3f));
	pMenu->addChild(btnExtras);
#endif
    
	float d = 0.25f;
	float flip = 1;
	for (auto node : pMenu->getChildren())
	{
		float targetX = node->getPositionX();
		node->setPositionX(targetX + flip * PX(1.0f));
		flip *= -1;
		
		node->runAction(Sequence::createWithTwoActions(DelayTime::create(d += 0.35f), MoveTo::create(0.75f, Point(targetX, node->getPositionY()))));
	}
    
    auto hooker = TopAnchor::create();
    addChild(hooker, 1);
	Menu* hookerMenu = Menu::create();
	hookerMenu->setPosition(Point::ZERO);
	hooker->addChild(hookerMenu, 1);

#if (CC_TARGET_PLATFORM != CC_PLATFORM_ANDROID)
	/////////// SOUND
	string sprite = GameSettings::MuteSounds ? "btnMute" : "btnSound";
	string psprite = sprite + "Dn";
	sprite += "Up";
	btnSound = MAKEBUTTON(sprite, psprite, btnSoundPressed);
	btnSound->setScale(1.3f);
	hookerMenu->addChild(btnSound);
	btnSound->setAnchorPoint(Point(1.5, 1.5));
	btnSound->setPosition(VCP(1.0f, 1.0f));
#endif
    
	auto keyboard = EventListenerKeyboard::create();
	keyboard->onKeyReleased = CC_CALLBACK_2(MainMenu::onKeyReleased, this);
	getEventDispatcher()->addEventListenerWithSceneGraphPriority(keyboard, this);
    
    scheduleUpdate();
    return true;
}


void MainMenu::btnNewGamePressed(Ref*)
{
	Director::getInstance()->replaceScene(TransitionFlipY::create(0.5f, EpisodeChooser::createScene()));
}

void MainMenu::btnHelpPressed(Ref*)
{
	Director::getInstance()->pushScene(TransitionFade::create(1.0f, BundleStore::createPreStoreScene()));
}

void MainMenu::btnCreditsPressed(Ref*)
{
}

void MainMenu::btnExtrasPressed(Ref*)
{
	Director::getInstance()->replaceScene(TransitionZoomFlipAngular::create(1.5f, BundleGamesScene::createScene()));
}

void MainMenu::btnExitPressed(Ref*){
	menuCloseCallback(nullptr);
}

void MainMenu::btnScoresPressed(Ref*){
#if IS_IOS
    GKHWrapperCpp gkh;
    gkh.showLeaderBoard();
#endif
}

void MainMenu::btnAchievementsPressed(Ref*)
{
#if IS_IOS
    GKHWrapperCpp gkh;
    gkh.showAchievements();
#endif
}

void MainMenu::menuCloseCallback(Ref*){

	PlayEffect("bottle_pop_2.wav");
	Director::getInstance()->end();

	#if (CC_TARGET_PLATFORM == CC_PLATFORM_IOS)
		exit(0);
	#endif
}

void MainMenu::btnSoundPressed(Ref*)
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

void MainMenu::onKeyReleased(EventKeyboard::KeyCode keyCode, Event* event)
{
	if (keyCode == EventKeyboard::KeyCode::KEY_ESCAPE)
		btnExitPressed(this);
}