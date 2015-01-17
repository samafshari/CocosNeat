#include "DronesGameOverScene.h"
#include "helpers.h"
#include "GameSettings.h"
#include "DronesScene.h"
#include "MainMenuScene.h"
#include "GameCenter.h"
#include "BackgroundWidget.h"
#include "Widgets.h"

using namespace Drones;

Scene* GameOverScene::createScene()
{
    auto scene = Scene::create();
	auto layer = GameOverScene::create();
    scene->addChild(layer);
    return scene;
}

// on "init" you need to initialize your instance
bool GameOverScene::init()
{
    //////////////////////////////
    // 1. super init first
    if ( !Layer::init() )
    {
        return false;
    }
    
    Size visibleSize = Director::getInstance()->getVisibleSize();
    
	bool isHighscore = GameSettings::DronesLastScore >= GameSettings::DronesHighScore;

	AdsEnabled = true;
	if (!isHighscore) ShowFullscreenAd();
    
	auto bgcolor = MAKESPRITE("bg");
	bgcolor->setPosition(visibleSize.width * 0.5f, visibleSize.height * 0.5f);
	bgcolor->setAnchorPoint(Point(0.5f, 0.5f));
	bgcolor->setColor(Color3B(20, 20, 80));
	addChild(bgcolor);
	bgcolor->setOpacity(0);
	bgcolor->runAction(
		Sequence::create(
		MoveBy::create(0.1f, Point::ZERO),
		FadeIn::create(0.5f),
		MoveBy::create(3.0f, Point::ZERO),
		NULL));

    addChild(BackgroundWidget::create("bg", Color3B(20, 20, 80)));

	auto vig = Sprite::create("vig.png");
	auto vigSize = vig->getContentSize();
	vig->setScaleX(visibleSize.width / vigSize.width);
	vig->setScaleY(visibleSize.height / vigSize.height);
	vig->setPosition(CP(0.5f, 0.5f));
	addChild(vig);

	auto timeout = Label::createWithSystemFont("Game Over", "Arial", 46.0f);
	timeout->setHorizontalAlignment(TextHAlignment::CENTER);
	timeout->setAnchorPoint(Point(0.5f, 1.0f));
	timeout->setPosition(CP(0.5f, 0.6f));
	timeout->setColor(Color3B::WHITE);
	timeout->setTag(11);
	addChild(timeout);

	auto scorelbl = Label::createWithSystemFont("High Score:", "Arial", 42.0f);
	// + nToString(GameSettings::DronesHighScore)
	scorelbl->setHorizontalAlignment(TextHAlignment::LEFT);
	scorelbl->setAnchorPoint(Point(0.0f, 1.0f));
	scorelbl->setPosition(CP(0.10, 0.5f));
	scorelbl->setColor(Color3B::WHITE);
	scorelbl->setTag(12);
	addChild(scorelbl);

	if (GameSettings::DronesLastScore >= GameSettings::DronesHighScore)
	{
		GameSettings::DronesHighScore = GameSettings::DronesLastScore;
		GameSettings::SaveSettings();
		scorelbl->setString("You broke the high score!");
		scorelbl->setPositionX(PX(0.5f));
		scorelbl->setAnchorPoint(Point(0.5f, 1.0f));
		scorelbl->setHorizontalAlignment(TextHAlignment::CENTER);
		Node* n = Node::create();
		addChild(n);
		n->setTag(14);
	}
	else
	{
		auto scoreVal = Label::createWithSystemFont(nToString(GameSettings::DronesHighScore), "Arial", 42.0f);
		scoreVal->setAnchorPoint(Point(1.0f, 1.0f));
		scoreVal->setPosition(CP(0.9f, 0.5f));
		scoreVal->setHorizontalAlignment(TextHAlignment::RIGHT);
		scoreVal->setColor(Color3B::WHITE);
		scoreVal->setTag(14);
		addChild(scoreVal);
	}

	auto scorelbl2 = Label::createWithSystemFont("Your Score:", "Arial", 42.0f);
	scorelbl2->setHorizontalAlignment(TextHAlignment::LEFT);
	scorelbl2->setAnchorPoint(Point(0.0f, 1.0f));
	scorelbl2->setPosition(CP(0.1f, 0.43f));
	scorelbl2->setColor(Color3B::WHITE);
	scorelbl2->setTag(13);
	addChild(scorelbl2);

	auto scoreVal2 = Label::createWithSystemFont(nToString(GameSettings::DronesLastScore), "Arial", 42.0f);// LabelBMFont::create(nToString(GameSettings::DronesLastScore), "naskh.fnt");
	scoreVal2->setHorizontalAlignment(TextHAlignment::RIGHT);
	scoreVal2->setAnchorPoint(Point(1.0f, 1.0f));
	scoreVal2->setPosition(CP(0.9f, 0.43f));
	scoreVal2->setColor(Color3B::WHITE);
	scoreVal2->setTag(15);
	addChild(scoreVal2);

	float delay = 0.5f;
	for (int i = 11; i <= 15; i++,delay+=0.5f)
	{
		getChildByTag(i)->setOpacity(0);
		getChildByTag(i)->runAction(Sequence::createWithTwoActions(DelayTime::create(delay),
			FadeIn::create(0.25f)));
	}
/*
	Node* facebookShareButton = FacebookShareButton::create(isHighscore);
	facebookShareButton->setPosition(CP(0.5f, -0.5f));
	facebookShareButton->runAction(MoveTo::create(1.0f, CP(0.5f, 0.1f)));
	facebookShareButton->setAnchorPoint(Point(0.5f, 0));
	addChild(facebookShareButton);
*/
	scheduleOnce(schedule_selector(GameOverScene::advanceScreen), 4.0f);

	auto touch = EventListenerTouchOneByOne::create();
	touch->onTouchBegan = CC_CALLBACK_2(GameOverScene::onTouchBegan, this);
	getEventDispatcher()->addEventListenerWithSceneGraphPriority(touch, this);
	PlayEffect("gameover.wav");

    return true;
}

void GameOverScene::menuCloseCallback(Ref* pSender)
{
}

bool GameOverScene::onTouchBegan(Touch*, Event*)
{
	unscheduleAllSelectors();
	advanceScreen();
	return true;
}

void GameOverScene::advanceScreen(float)
{
	/*Director::getInstance()->replaceScene(TransitionFade::create(0.7f,
		MainMenu::scene()
		));*/
    Director::getInstance()->popScene();
}