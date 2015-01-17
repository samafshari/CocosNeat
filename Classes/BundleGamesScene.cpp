#include "helpers.h"
#include "BundleGamesScene.h"
#include "BackgroundWidget.h"
#include "MeshScene.h"
#include "SnakeScene.h"
#include "TreeScene.h"
#include "FallScene.h"
#include "Raid.h"
#include "DonkeyScene.h"
#include "Widgets.h"
#include "DronesScene.h"
#include "ExtraLockedScene.h"
#include "GameInfoScene.h"
#include "EpisodeChooserScene.h"
#include "Blob.h"
#include "BundleMainMenuScene.h"
#if !IS_IOS
#include "MainMenuScene.h"
#endif
#include "Sidebar.h"
#include "MiniMainMenu.h"
#include "Platformer.h"
#include "MelodieScene.h"
#include "MatryoshkaScene.h"
#include "ContrastScene.h"
#include "BlobMenu.h"
#include "Mesh2Scene.h"
static inline MenuItemSprite* MakeIcon(string spriteName, string text, float fontSize, function<void(Ref*)> f)
{
	auto up = MAKESPRITE(spriteName);
	auto upRect = up->getContentSize();

	auto lblUp = Label::createWithTTF(text, NORMAL_TTF, fontSize); //Label::createWithSystemFont(text, "Arial", fontSize);
	lblUp->setHorizontalAlignment(TextHAlignment::CENTER);
	lblUp->setAnchorPoint(Point(0.5f, 1.0f));
	lblUp->setPosition(Point(upRect.width * 0.5f, 0));
	up->addChild(lblUp);

	auto dn = MAKESPRITE(spriteName);
	dn->setColor(Color3B(150, 150, 150));
	auto dnRect = dn->getContentSize();

	auto lblDn = Label::createWithTTF(text, NORMAL_TTF, fontSize);
	lblDn->setHorizontalAlignment(TextHAlignment::CENTER);
	lblDn->setAnchorPoint(Point(0.5f, 1.0f));
	lblDn->setPosition(Point(dnRect.width * 0.5f, 0));
	dn->addChild(lblDn);

	auto result = MenuItemSprite::create(up, dn, f);
	result->setAnchorPoint(Point(0.5f, 0));
	return result;
}

Scene* BundleGamesScene::createScene()
{
    Scene *scene = Scene::create();
	BundleGamesScene *layer = BundleGamesScene::create();
    scene->addChild(layer);
    return scene;
}

bool BundleGamesScene::init()
{
    if ( !Layer::init() )
    {
        return false;
    }
    
    AdsEnabled = false;
    
	Size visibleSize = Director::getInstance()->getVisibleSize();

#ifdef NEAT_BUNDLE
	Color4B bgcolor_color = Color4B::GRAY;
#else
	Color4B bgcolor_color = Color4B::BLACK;
#endif
	auto bgcolor = LayerColor::create(bgcolor_color, visibleSize.width, visibleSize.height);
	bgcolor->setPosition(Point::ZERO);
	bgcolor->setTag(2000);
	addChild(bgcolor);

	auto particles = ParticleSystemQuad::create("magic/particle.plist");
	particles->setPosition(CP(0.5f, 0.5f));
	addChild(particles);
	
	addChild(BackgroundWidget::createWithoutBackground());
	addChild(Vignette::create());

	float fontSize = 32;
    
    auto hooker = TopAnchor::create();
    addChild(hooker, 1);
    auto hookerMenu = Menu::create();
    hookerMenu->setPosition(Point::ZERO);
    hooker->addChild(hookerMenu);
    
#if (CC_TARGET_PLATFORM != CC_PLATFORM_ANDROID)
	auto btnMenu = MAKEBUTTON("btnBackUp", "btnBackDn", BundleGamesScene::menuCloseCallback);
	btnMenu->setPosition(VCP(0.03f, 0.97f));
	btnMenu->setAnchorPoint(Point(0, 1.0f));
	btnMenu->setScale(1.3f);
	hookerMenu->addChild(btnMenu);
#endif
    
	auto pMenu = Menu::create();
	pMenu->setPosition(Point::ZERO);
	addChild(pMenu, 1);

	auto grayBar = LayerColor::create(Color4B(0, 0, 0, 0), DESIGN_X, PY(0.6f));
	grayBar->setPosition(CP(0.0f, 0.17f));
	addChild(grayBar);

	auto btnMesh = MakeIcon(GameSettings::MeshUnlocked ? "btnMesh" : "btnLock", 
		LS("Mesh"), fontSize, [](Ref*){TryRunGame(Games::Mesh); });
	btnMesh->setPosition(CP(0.2f, 0.7f));
	pMenu->addChild(btnMesh);

	auto btnNaturals = MakeIcon(GameSettings::NaturalsUnlocked ? "btnNaturals" : "btnLock",
		LS("Naturals")+"*", fontSize, [](Ref*){OpenNaturalsPage(); });
	btnNaturals->setPosition(CP(0.5f, 0.7f));
	pMenu->addChild(btnNaturals);

	auto btnSnake = MakeIcon(GameSettings::SnakeUnlocked ? "btnSnake" : "btnLock",
		LS("Snake"), fontSize, [](Ref*){TryRunGame(Games::Snake); });
	btnSnake->setPosition(CP(0.8f, 0.7f));
	pMenu->addChild(btnSnake);

	auto btnRaid = MakeIcon(GameSettings::RaidUnlocked ? "btnRaid" : "btnLock",
		LS("Raid"), fontSize, [](Ref*){TryRunGame(Games::Raid); });
	btnRaid->setPosition(CP(0.2f, 0.45f));
    //btnRaid->setAnchorPoint(Point(0.5f, 1));
	pMenu->addChild(btnRaid);

	auto btnTree = MakeIcon(GameSettings::TreeUnlocked ? "btnTree" : "btnLock",
		LS("TreeMassacre"), fontSize, [](Ref*){TryRunGame(Games::Tree); });
	btnTree->setPosition(CP(0.5f, 0.45f));
    //btnTree->setAnchorPoint(Point(0.5f, 1));
	pMenu->addChild(btnTree);

	auto btnFall = MakeIcon(GameSettings::FallUnlocked ? "btnFall" : "btnLock",
		LS("Fall"), fontSize, [](Ref*){TryRunGame(Games::Fall); });
	btnFall->setPosition(CP(0.8f, 0.45f));
    //btnFall->setAnchorPoint(Point(0.5f, 1));
	pMenu->addChild(btnFall);
    
	auto btnBlob = MakeIcon("btnBlob", LS("Blob"), fontSize,
		[](Ref*){TryRunGame(Games::Blob);});
	btnBlob->setPosition(CP(0.2f, 0.2f));
	//btnBlob->setAnchorPoint(Point(0.5f, 0));
	pMenu->addChild(btnBlob);

	auto btnContrast = MakeIcon("btnContrast", LS("Contrastic"), fontSize,
		[](Ref*){TryRunGame(Games::Contrast); });
	btnContrast->setPosition(CP(0.5f, 0.2f));
	//btnContrast->setAnchorPoint(Point(0.5f, 0));
	pMenu->addChild(btnContrast);

	auto btnMesh2 = MakeIcon("btnMesh2", LS("Mesh2"), fontSize,
		[](Ref*){TryRunGame(Games::Mesh2); });
	btnMesh2->setPosition(CP(0.8f, 0.2f));
	//btnMesh2->setAnchorPoint(Point(0.5f, 0));
	pMenu->addChild(btnMesh2);

#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)
    pMenu->setPositionY(pMenu->getPositionY() + PY(0.05f));
    grayBar->setPositionY(grayBar->getPositionY() + PY(0.05f));

	addChild(HBar::CreateWithStandardButtons());
#endif
    
	auto keyboard = EventListenerKeyboard::create();
	keyboard->onKeyReleased = CC_CALLBACK_2(BundleGamesScene::onKeyReleased, this);
	getEventDispatcher()->addEventListenerWithSceneGraphPriority(keyboard, this);

    
	//schedule(schedule_selector(BundleGamesScene::changeColor), 7.0f);
	//changeColor(0);
    return true;
}

void BundleGamesScene::changeColor(float)
{
	auto nextColor = hsl2Rgb(RANDF * 0.8f, 0.7f, 0.3f);
	getChildByTag(2000)->runAction(TintTo::create(5.0f, nextColor.r, nextColor.g, nextColor.b));
}


void BundleGamesScene::menuCloseCallback(Ref*){

#ifdef NEAT_BUNDLE
	Director::getInstance()->replaceScene(TransitionFade::create(0.5f, BundleMainMenu::scene()));
#else
	Director::getInstance()->replaceScene(TransitionFade::create(0.5f,
                                                                 EpisodeChooser::createScene()));
#endif
}

void BundleGamesScene::onKeyReleased(EventKeyboard::KeyCode keyCode, Event* event)
{
	if (keyCode == EventKeyboard::KeyCode::KEY_ESCAPE)
		menuCloseCallback(this);
	if (keyCode == EventKeyboard::KeyCode::KEY_F1)
		GameSettings::GiveGems(1);
	if (keyCode == EventKeyboard::KeyCode::KEY_F2)
		GameSettings::GiveGems(10);
}

void BundleGamesScene::btnDronesPressed(Ref*)
{
	Director::getInstance()->pushScene(TransitionRotoZoom::create(1.0f, Drones::DronesScene::StartEggplantGame()));
}

Scene* BundleGamesScene::TryRunGame(Games game, bool act)
{
	bool unlocked = true;
	if (game == Games::Mesh) unlocked = GameSettings::MeshUnlocked;
	else if (game == Games::Tree) unlocked = GameSettings::TreeUnlocked;
	else if (game == Games::Snake) unlocked = GameSettings::SnakeUnlocked;
	else if (game == Games::Raid) unlocked = GameSettings::RaidUnlocked;
	else if (game == Games::Fall) unlocked = GameSettings::FallUnlocked;
	else if (game == Games::Donkey) unlocked = GameSettings::DonkeyUnlocked;

	Scene* target = nullptr;
	bool push = true;

	if (game == Games::Blob)
		target = Blob::BlobMenu::createScene(Games::Blob);
	else if (game == Games::Matryoshka)
		target = Matryoshka::MatryoshkaScene::CreateMenu(Games::Matryoshka);
	else if (game == Games::Platformer)
		target = Platformer::ChooserScene::scene();
	else if (game == Games::Mesh2)
		target = Mesh2::Mesh2Scene::scene();
	else
	{
		if (unlocked)
			target = TransitionFade::create(1.0f, 
			MiniMainMenu::createScene(game, Director::getInstance()->getRunningScene()));
		else
		{
			push = false;
			target = (TransitionFlipY::create(1.0f,
				ExtraLockedScene::createScene(game)));
		}
	}

	if (act && target != nullptr)
	{
		if (push) Director::getInstance()->pushScene(target);
		else Director::getInstance()->replaceScene(target);
	}

	return target;
}

void BundleGamesScene::RunGame(Games game)
{
#if 0 //disable gem feature
    #if (CC_TARGET_PLATFORM != CC_PLATFORM_ANDROID)
	if (!GameSettings::SpendGems(1))
	{
#if IS_IOS
		ShowPopup("Oops", "You don't have enough gems\nto play this game.\nBetter earn some!");
		return;
#endif
	}
#endif
#endif
    
	Scene* scene = nullptr;

	if (game == Games::Mesh) scene = MeshScene::scene();
	else if (game == Games::Tree) scene = TreeScene::scene();
	else if (game == Games::Snake) scene = Snake::SnakeScene::scene();
	else if (game == Games::Raid) scene = Raid::RaidScene::scene();
	else if (game == Games::Fall) scene = FallScene::createScene();
	else if (game == Games::Drones) scene = Drones::DronesScene::StartRandomGame();
	else if (game == Games::Donkey) scene = DonkeyScene::createScene();
	else if (game == Games::Melodie) scene = Melodie::MelodieScene::scene();
	else if (game == Games::Blob) scene = Blob::BlobScene::scene();
	else if (game == Games::Matryoshka) scene = Matryoshka::MatryoshkaScene::scene();
	else if (game == Games::Platformer) scene = Platformer::ChooserScene::scene();
	else if (game == Games::Contrast) scene = Contrast::ContrastScene::createScene();
	else if (game == Games::DronesEggplant) scene = Drones::DronesScene::StartEggplantGame();
	else if (game == Games::DronesRandom) scene = Drones::DronesScene::StartRandomGame();

	if (scene) Director::getInstance()->pushScene(TransitionFade::create(0.5f, scene));
}