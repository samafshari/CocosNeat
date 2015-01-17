#include "EpilogueScene.h"
#include "NeatScene.h"
#include "helpers.h"
#include "GameSettings.h"
#include "Widgets.h"

Scene* EpilogueScene::createScene()
{
    auto scene = Scene::create();
	auto layer = EpilogueScene::create();
    scene->addChild(layer);
    return scene;
}

// on "init" you need to initialize your instance
bool EpilogueScene::init()
{
    //////////////////////////////
    // 1. super init first
    if ( !Layer::init() )
    {
        return false;
    }
    
    Size visibleSize = Director::getInstance()->getVisibleSize();
	
	auto bgcolor = MAKESPRITE("bg" + nToString(GameSettings::CurrentEpisode));
	bgcolor->setPosition(visibleSize.width * 0.5f, visibleSize.height * 0.5);
	bgcolor->setAnchorPoint(Point(0.5f, 0.5f));
	bgcolor->setColor(Color3B(200, 200, 200));
	addChild(bgcolor);
	bgcolor->setOpacity(0);
	bgcolor->runAction(
		Sequence::create(
		MoveBy::create(0.1f, Point::ZERO),
		FadeIn::create(0.5f),
		MoveBy::create(3.0f, Point::ZERO),
		NULL)); 
	bgcolor->setTag(1);

	addChild(Vignette::create());

	auto hole = MAKESPRITE("blackholebig");
	addChild(hole);
	hole->setPosition(CP(0.5f, 0.8f));
	hole->setScale(0.8f);
	hole->runAction(RepeatForever::create(RotateBy::create(40.0, 365)));

	auto gamelogo = MAKESPRITE("gamelogo");
	addChild(gamelogo);
	gamelogo->setPosition(CP(0.5f, 0.8f));
	gamelogo->setScale(0.8f);
	gamelogo->runAction(RepeatForever::create(Sequence::createWithTwoActions(
		ScaleTo::create(3.0f, 0.81f),
		ScaleTo::create(3.0f, 0.79f)
		)));

	auto lbl1 = Label::createWithTTF(LS("Congratulations"), FANCY_TTF, 72.0f);// MAKESPRITE("end1");
	lbl1->setPosition(CP(0.5f, 1.55f));
	lbl1->setOpacity(0);
	lbl1->runAction(CCFadeIn::create(0.5f));
	lbl1->setHorizontalAlignment(TextHAlignment::CENTER);
	lbl1->runAction(MoveBy::create(1.0f, CP(0, -1.0f)));
	addChild(lbl1);

	auto lbl3 = Label::createWithTTF(LS("EpisodeFinishedText"), FANCY_TTF, 38.0f);
	lbl3->setPosition(CP(0.5f, 0.30f - 1.0f));
	lbl3->setOpacity(0);
	lbl3->runAction(CCFadeIn::create(0.5f));
	lbl3->runAction(MoveBy::create(1.0f, CP(0, 1.0f)));
	lbl3->setHorizontalAlignment(TextHAlignment::CENTER);
	addChild(lbl3);
	lbl3->enableShadow(Color4B::BLACK, Size(0.5f, -1.0f), 1);

#ifndef FA
	auto facebook = MAKESPRITE("facebook");
	addChild(facebook);
	facebook->setPosition(CP(0.5f, 0.1f));
	facebook->setOpacity(0);
	facebook->runAction(
		Sequence::create(
		MoveBy::create(1.7f, Point::ZERO),
		FadeIn::create(0.7f),
		MoveBy::create(2.0f, Point::ZERO),
		NULL));
#endif

	auto touch = EventListenerTouchOneByOne::create();
	touch->onTouchBegan = CC_CALLBACK_2(EpilogueScene::onTouchBegan, this);
	getEventDispatcher()->addEventListenerWithSceneGraphPriority(touch, this);

	schedule(schedule_selector(EpilogueScene::changeColor), 3.0f);
    
	PlayEffect("win.wav");
	return true;
}

void EpilogueScene::changeColor(float)
{
	auto nextColor = hsl2Rgb(RANDF * 0.8f, 0.7f, 0.3f);
	getChildByTag(1)->runAction(TintTo::create(1.75f, nextColor.r, nextColor.g, nextColor.b));
}

void EpilogueScene::menuCloseCallback(Ref* pSender)
{
	advanceScreen();
}

bool EpilogueScene::onTouchBegan(Touch*, Event*)
{
	advanceScreen();
	return true;
}

void EpilogueScene::advanceScreen(float)
{
	//Create a new game
	GameSettings::CurrentLevelId = 0;
	GameSettings::SaveSettings();

	Director::getInstance()->replaceScene(NeatScene::createScene());
}