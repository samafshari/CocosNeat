#include "helpers.h"
#include "EpisodeChooserScene.h"
#include "MainMenuScene.h"
#include "CreditsScene.h"
#include "ChooserScene.h"
#include "HelpScene.h"
#include "BackgroundWidget.h"
#include "EpisodeLockedScene.h"
#include "Widgets.h"
#include "BundleGamesScene.h"
#include "DronesScene.h"
#include "Widgets.h"
#include "Sidebar.h"
#include "ShaderLayer.h"

Scene* EpisodeChooser::createScene()
{
    Scene *scene = Scene::create();
	EpisodeChooser *layer = EpisodeChooser::create();
    scene->addChild(layer);
    return scene;
}

bool EpisodeChooser::init()
{
    if ( !Layer::init() )
    {
        return false;
    }
    
	Size visibleSize = Director::getInstance()->getVisibleSize();

	auto shader = ShaderLayer::create("blur.glsl", "blur.vsh");
	addChild(shader);

	shader->addChild(BackgroundWidget::createWithDefaultBackground(Color3B(20, 20, 100)));

	shader->addChild(Vignette::create());
   
//#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)
	auto particles = ParticleSystemQuad::create("magic/particle.plist");
	particles->setPosition(CP(0.5f, 0.5f));
	shader->addChild(particles);
//#endif

    auto hooker = TopAnchor::create();
    addChild(hooker, 1);
	Menu* hookerMenu = Menu::create();
	hookerMenu->setPosition(Point::ZERO);
	hooker->addChild(hookerMenu, 1);
    
	/////////// MENU
	auto pMenu = Menu::create(); 
	pMenu->setPosition(Point::ZERO);
	addChild(pMenu, 1);

	float fontSize = 38.0f;

	vector<Node*> buttons;
	
	auto btnEp1 = MakeButton("btnUp", "btnDn", LS("BTNEpisode1"), fontSize, CC_CALLBACK_1(EpisodeChooser::btnEpisodePressed, this));
	btnEp1->setTag(0);
	buttons.push_back(btnEp1);

	auto btnEp2 = MakeButton("btnUp", "btnDn", LS("BTNEpisode2"), fontSize, CC_CALLBACK_1(EpisodeChooser::btnEpisodePressed, this));
	btnEp2->setTag(1);
	buttons.push_back(btnEp2);

	auto btnEp3 = MakeButton("btnUp", "btnDn", LS("BTNEpisode3"), fontSize, CC_CALLBACK_1(EpisodeChooser::btnEpisodePressed, this));
	btnEp3->setTag(2);
	buttons.push_back(btnEp3);

	auto btnEggplant = MakeButton("btnUp", "btnDn", LS("BTNArcadeMode"), fontSize, CC_CALLBACK_1(EpisodeChooser::btnEggplantPressed, this));
	buttons.push_back(btnEggplant);

#ifndef WIN32
	if (GameSettings::DronesArcadeUnlocked)
#endif
	{
		auto btnArcade = MakeButton("btnUp", "btnDn", LS("BTNRandomLevels"), fontSize, CC_CALLBACK_1(EpisodeChooser::btnArcadePressed, this));
		buttons.push_back(btnArcade);
	}
    
	auto btnExtras = MakeButton("btnUp", "btnDn", LS("BTNGoodStuff"), fontSize, CC_CALLBACK_1(EpisodeChooser::btnExtrasPressed, this));
	buttons.push_back(btnExtras);
	recursiveAction(btnExtras, [](Node* base){base->setColor(Color3B(220,255,20)); });

	Size btnSize = MAKESPRITE("btnUp")->getContentSize();
	btnSize.height *= 1.1f;
	float y = (buttons.size() * btnSize.height) * 0.5f + PY(0.5f);
    
	auto gamelogotext = MAKESPRITE("gamelogotext");
	gamelogotext->setPosition(PX(0.5f),y);
	gamelogotext->setAnchorPoint(Point(0.5f, 0.3f));
	
	auto redbg = LayerColor::create(Color4B(127, 0, 0, 255), DESIGN_X, gamelogotext->getContentSize().height);
	redbg->ignoreAnchorPointForPosition(false);
	redbg->setAnchorPoint(gamelogotext->getAnchorPoint());
	redbg->setPosition(gamelogotext->getPosition());

	addChild(redbg);
	addChild(gamelogotext);
    
	y -= btnSize.height;

	for (auto btn : buttons)
	{
		pMenu->addChild(btn);
		btn->setPosition(PX(0.5f), y);
		y -= btnSize.height;
	}
    
	addChild(HBar::CreateWithStandardButtons());
	
#if IS_IOS
	auto timer = GemTimerLayer::create();
	timer->setAnchorPoint(Point(1.0f, 0.0f));
	timer->setPosition(CP(0.95f, 0.02f));
	addChild(timer);
#endif
    
	auto keyboard = EventListenerKeyboard::create();
	keyboard->onKeyReleased = CC_CALLBACK_2(EpisodeChooser::onKeyReleased, this);
	getEventDispatcher()->addEventListenerWithSceneGraphPriority(keyboard, this);

    return true;
}

void EpisodeChooser::btnEggplantPressed(Ref*)
{
	BundleGamesScene::TryRunGame(Games::DronesEggplant);
}

void EpisodeChooser::btnArcadePressed(Ref*)
{
	BundleGamesScene::TryRunGame(Games::DronesRandom);
}

void EpisodeChooser::btnEpisodePressed(Ref* r)
{
	GameSettings::CurrentEpisode = static_cast<Node*>(r)->getTag();
	if (GameSettings::CurrentEpisode > 0 &&
		GameSettings::FinishedEpisodes.find(GameSettings::CurrentEpisode - 1) ==
		GameSettings::FinishedEpisodes.end())
	{
		//episode is locked
		Director::getInstance()->replaceScene(TransitionFade::create(0.5f, EpisodeLockedScene::createScene()));
	}
	else
	{
		GameSettings::CurrentLevelId = 0;
		Director::getInstance()->replaceScene(TransitionFade::create(0.5f, ChooserScene::scene()));
		GameSettings::PlayMusic(EpisodeInfos[GameSettings::CurrentEpisode].music);
	}
}

void EpisodeChooser::btnExtrasPressed(Ref*)
{
	Director::getInstance()->replaceScene(TransitionFade::create(1.5f, BundleGamesScene::createScene()));
	GameSettings::StopMusic();
}

void EpisodeChooser::menuCloseCallback(Ref*){

#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)
    Director::getInstance()->end();
    return;
#endif
	Director::getInstance()->replaceScene(TransitionFlipY::create(0.5f, MainMenu::scene()));
}

void EpisodeChooser::btnSoundPressed(Ref*)
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

void EpisodeChooser::onKeyReleased(EventKeyboard::KeyCode keyCode, Event* event)
{
	if (keyCode == EventKeyboard::KeyCode::KEY_ESCAPE)
		menuCloseCallback(this);
}