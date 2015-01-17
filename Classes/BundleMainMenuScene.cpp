#include "helpers.h"
#include "BundleMainMenuScene.h"
#include "CreditsScene.h"
#include "ChooserScene.h"
#include "HelpScene.h"
#include "BackgroundWidget.h"
#include "EpisodeChooserScene.h"
#include "BundleGamesScene.h"
#include "BundleStoreScene.h"

Scene* BundleMainMenu::scene()
{
    Scene *scene = Scene::create();
    BundleMainMenu *layer = BundleMainMenu::create();
    scene->addChild(layer);
    return scene;
}

bool BundleMainMenu::init()
{
    if ( !Layer::init() )
    {
        return false;
    }
    
	Size visibleSize = Director::getInstance()->getVisibleSize();
    
	auto bg = LayerColor::create(Color4B::WHITE, visibleSize.width, visibleSize.height);
	this->addChild(bg);
/*
	auto bgcolor = MAKESPRITE("bg");
	bgcolor->setPosition(visibleSize.width * 0.5f, visibleSize.height * 0.5f);
	bgcolor->setAnchorPoint(Point(0.5f, 0.5f));
	bgcolor->setColor(Color3B(20, 20, 100));
	addChild(bgcolor);*/

	addChild(BackgroundWidget::createWithoutBackground());

	auto vig = Sprite::create("vig.png");
	auto vigSize = vig->getContentSize();
	vig->setScaleX(visibleSize.width / vigSize.width);
	vig->setScaleY(visibleSize.height / vigSize.height);
	vig->setPosition(CP(0.5f, 0.5f));
	addChild(vig);
	
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

	auto btnNewGame = MakeButton("btnUp", "btnDn", "Play", fontSize, CC_CALLBACK_1(BundleMainMenu::btnNewGamePressed, this));
	btnNewGame->setPosition(CP(0.5f, 0.4f));
	pMenu->addChild(btnNewGame);

	auto btnHelp = MakeButton("btnUp", "btnDn", "Get Gems", fontSize, CC_CALLBACK_1(BundleMainMenu::btnStorePressed, this));
	btnHelp->setPosition(CP(0.5f, 0.3f));
	pMenu->addChild(btnHelp);

	auto btnScores = MakeButton("btnUp", "btnDn", "High Scores", fontSize, CC_CALLBACK_1(BundleMainMenu::btnScoresPressed, this));
	btnScores->setPosition(CP(0.5f, 0.2f));
	pMenu->addChild(btnScores);

	auto btnAchievements = MakeButton("btnUp", "btnDn", "Achievements", fontSize, CC_CALLBACK_1(BundleMainMenu::btnAchievementsPressed, this));
	btnAchievements->setPosition(CP(0.5f, 0.1f));
	pMenu->addChild(btnAchievements);

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
    auto hookerMenu = Menu::create();
    hookerMenu->setPosition(Point::ZERO);
    hooker->addChild(hookerMenu);
	/////////// SOUND
	string sprite = GameSettings::MuteSounds ? "btnMute" : "btnSound";
	string psprite = sprite + "Dn";
	sprite += "Up";
	btnSound = MAKEBUTTON(sprite, psprite, btnSoundPressed);
	btnSound->setScale(1.3f);
	hookerMenu->addChild(btnSound);
	btnSound->setAnchorPoint(Point(1.5, 1.5));
	btnSound->setPosition(VCP(1.0f, 1.0f));
	
	auto keyboard = EventListenerKeyboard::create();
	keyboard->onKeyReleased = CC_CALLBACK_2(BundleMainMenu::onKeyReleased, this);
	getEventDispatcher()->addEventListenerWithSceneGraphPriority(keyboard, this);

    scheduleUpdate();
    
    
    EngineRunning = true;
    return true;
}


void BundleMainMenu::btnNewGamePressed(Ref*)
{
	Director::getInstance()->replaceScene(
		TransitionFade::create(0.5f, BundleGamesScene::createScene()));
}

void BundleMainMenu::btnStorePressed(Ref*)
{
    Director::getInstance()->pushScene(TransitionFade::create(1.0f, BundleStore::createPreStoreScene()));
}

void BundleMainMenu::btnExitPressed(Ref*){
	menuCloseCallback(nullptr);
}

void BundleMainMenu::btnScoresPressed(Ref*){
#if IS_IOS
    GKHWrapperCpp gkh;
    gkh.showLeaderBoard();
#endif
}

void BundleMainMenu::btnAchievementsPressed(Ref*)
{
#if IS_IOS
    GKHWrapperCpp gkh;
    gkh.showAchievements();
#endif
}

void BundleMainMenu::menuCloseCallback(Ref*){

	PlayEffect("bottle_pop_2.wav");
	Director::getInstance()->end();

	#if (CC_TARGET_PLATFORM == CC_PLATFORM_IOS)
		exit(0);
	#endif
}

void BundleMainMenu::btnSoundPressed(Ref*)
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

void BundleMainMenu::onKeyReleased(EventKeyboard::KeyCode keyCode, Event* event)
{
	if (keyCode == EventKeyboard::KeyCode::KEY_ESCAPE)
		btnExitPressed(this);
}