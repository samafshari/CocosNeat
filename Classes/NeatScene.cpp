#include "NeatScene.h"
#include "helpers.h"
#include "GameSettings.h"
#include "BackgroundWidget.h"

Scene* NeatScene::createScene()
{
    auto scene = Scene::create();
	auto layer = NeatScene::create();
    scene->addChild(layer);
    return scene;
}

// on "init" you need to initialize your instance
bool NeatScene::init()
{
    //////////////////////////////
    // 1. super init first
    if ( !Layer::init() )
    {
        return false;
    }
    
    Size visibleSize = Director::getInstance()->getVisibleSize();

	Color4B bgcolor = Color4B::BLACK;
#if defined(S_GAME)
	bgcolor = GameInfos[APPLICATION].neatColor;
#endif
	auto bg = LayerColor::create(bgcolor, visibleSize.width, visibleSize.height);
	bg->setOpacity(0);
	this->addChild(bg);
	bg->runAction(FadeIn::create(1.0f));
#if MINI_GAME
    //addChild(LayerColor::create(Color4B::BLACK, visibleSize.width, visibleSize.height));
	//addChild(BackgroundWidget::createWithoutBackground());
#else
#endif

	auto neatLogo = Sprite::createWithSpriteFrameName("neatlogo");
	addChild(neatLogo);
#if defined(LIGHT_THEME)
	neatLogo->setColor(Color3B::BLACK);
#endif
#if (ENABLE_IAD)
    neatLogo->setPosition(CP(0.5f, 0.51f));
#else
	neatLogo->setPosition(CP(0.5f, 0.5f));
#endif

	neatLogo->setOpacity(0);
	neatLogo->runAction(
		Sequence::create(
		DelayTime::create(0.7f),
		FadeIn::create(1.0),
		MoveBy::create(2.0f, Point::ZERO),
		NULL));

//#if !defined(LIGHT_THEME) //show facebook logo only in dark theme
//	auto facebook = MAKESPRITE("facebook");
//	addChild(facebook);
//	facebook->setPosition(CP(0.5f, 0.1f));
//	facebook->setOpacity(0);
//	facebook->runAction(
//		Sequence::create(
//		MoveBy::create(1.7f, Point::ZERO),
//		FadeIn::create(0.7f),
//		MoveBy::create(2.0f, Point::ZERO),
//		NULL));
//#endif

	auto source = Label::createWithTTF(
		"Get the source code for free\n"
		"at neat-games.com/source",
		NORMAL_TTF, 28.0f);
	source->setPosition(CP(0.5f, 0.1f));
	source->setColor(Color3B::WHITE);
	source->setHorizontalAlignment(TextHAlignment::CENTER);
	source->setOpacity(0);
	source->runAction(
		Sequence::create(
		MoveBy::create(1.0f, Point::ZERO),
		FadeIn::create(0.7f),
		MoveBy::create(2.0f, Point::ZERO),
		NULL));
	addChild(source);
	scheduleOnce(schedule_selector(NeatScene::advanceScreen), 4.5f);
#if defined(LIGHT_THEME)
	source->setColor(Color3B::BLACK);
#endif
    listener = EventListenerTouchOneByOne::create();
    listener->onTouchBegan = CC_CALLBACK_2(NeatScene::onTouchBegan, this);
    listener->onTouchEnded = CC_CALLBACK_2(NeatScene::onTouchEnded, this);
	getEventDispatcher()->addEventListenerWithSceneGraphPriority(listener, this);
	
    //call store init here
#if !MINI_GAME
    Purchase::initStore();
#endif
    return true;
}

void NeatScene::menuCloseCallback(Ref* pSender)
{
    Director::getInstance()->end();

#if (CC_TARGET_PLATFORM == CC_PLATFORM_IOS)
    exit(0);
#endif
}

void NeatScene::advanceScreen(float)
{
	Director::getInstance()->replaceScene(GetMainScene());
}

bool NeatScene::onTouchBegan(Touch* t, Event*)
{
    unscheduleAllSelectors();
    OpenUrl("http://neat-games.com/source");
    getEventDispatcher()->removeEventListener(listener);
    advanceScreen();
    return false;
}

void NeatScene::onTouchEnded(Touch*, Event*)
{
    
}