#include "DronesSplashScene.h"
#include "ChooserScene.h"
#include "EpisodeChooserScene.h"
#include "BackgroundWidget.h"
#include "Widgets.h"

Scene* DronesSplashScene::createScene()
{
    auto scene = Scene::create();
	auto layer = DronesSplashScene::create();
    scene->addChild(layer);
    return scene;
}

bool DronesSplashScene::init()
{
    if ( !Layer::init() )
    {
        return false;
    }
    
    Size visibleSize = Director::getInstance()->getVisibleSize();
	
	int ep = rand() % 4;
	GameSettings::PlayMusic(DRONES_MENU_MUSIC); // EpisodeInfos[ep].music);
	addChild(BackgroundWidget::create("bg" + nToString(ep), Color3B(200, 200, 200)));
	addChild(Vignette::create());

	auto hole = MAKESPRITE("blackholebig");
	addChild(hole);
	hole->setPosition(CP(0.5f, 0.6f));
	hole->runAction(RepeatForever::create(RotateBy::create(40.0, 365)));

	auto gamelogo = MAKESPRITE("gamelogo");
	addChild(gamelogo);
	gamelogo->setPosition(CP(0.5f, 0.6f));
	gamelogo->setScale(0.99f);
	gamelogo->runAction(RepeatForever::create(Sequence::createWithTwoActions(
		ScaleTo::create(3.0f, 1.02f),
		ScaleTo::create(3.0f, 0.99f)
		)));

	string title = LS(EpisodeInfos[GameSettings::CurrentEpisode].title);
	int price = EpisodeInfos[GameSettings::CurrentEpisode].price;

    auto hooker = TopAnchor::create();
    addChild(hooker, 1);

	auto hookerMenu = Menu::create();
	hookerMenu->setPosition(Point::ZERO);
	hooker->addChild(hookerMenu);
    
	auto pMenu = Menu::create();
	pMenu->setPosition(Point::ZERO);
	addChild(pMenu, 1);

	auto episodetext = Label::createWithTTF(LS("SplashText"), NORMAL_TTF, 40.0f);
	episodetext->setHorizontalAlignment(TextHAlignment::CENTER);
	episodetext->setPosition(VCP(0.5f, 0.30f));
	addChild(episodetext);
	
	auto black = LayerColor::create(Color4B::BLACK, DESIGN_X, DESIGN_Y);
	addChild(black);
	black->runAction(FadeOut::create(1.0f));

	auto keyboard = EventListenerKeyboard::create();
	keyboard->onKeyReleased = CC_CALLBACK_2(DronesSplashScene::onKeyReleased, this);
	getEventDispatcher()->addEventListenerWithSceneGraphPriority(keyboard, this);

	auto touch = EventListenerTouchOneByOne::create();
	touch->onTouchBegan = CC_CALLBACK_2(DronesSplashScene::onTouchBegan, this);
	getEventDispatcher()->addEventListenerWithSceneGraphPriority(touch, this);

    return true;
}

void DronesSplashScene::onKeyReleased(EventKeyboard::KeyCode keyCode, Event* event)
{
	if (keyCode == EventKeyboard::KeyCode::KEY_ESCAPE)
		menuCloseCallback(this);
}

void DronesSplashScene::menuCloseCallback(Ref* pSender)
{
	Director::getInstance()->replaceScene(TransitionFade::create(1.0f, EpisodeChooser::createScene()));
}

bool DronesSplashScene::onTouchBegan(Touch*, Event*)
{
	menuCloseCallback(this);
	return true;
}