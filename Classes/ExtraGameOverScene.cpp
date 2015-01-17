#include "ExtraGameOverScene.h"
#include "helpers.h"
#include "BackgroundWidget.h"
#include "Ooze.h"
#include "Widgets.h"

function<void()> ExtraGameOverScene::End = nullptr;


Scene* ExtraGameOverScene::createScene(Games game, Node* background)
{
	int score = *GameInfos[game].pLastScore;
	int highScore = *GameInfos[game].pHighScore;
	int totalScore = *GameInfos[game].pTotalScore;

	Ooze::InstantSend({
			{ "Event", "GameOverExtra" },
			{ "Score", nToString(score) },
			{ "HighScore", nToString(highScore) },
			{ "Application", GameInfos[APPLICATION].title }
	});
    auto scene = Scene::create();
	if (background) scene->addChild(background);

	auto layer = ExtraGameOverScene::create();
#if !defined(LOCAL_NAMEBOX)
	GameSettings::ReportScore(game, score);
#endif
	layer->score = score;
	layer->highScore = highScore;
	layer->totalScore = totalScore;
	layer->game = game;
	layer->initHighScore();
    scene->addChild(layer);

    return scene;
}

// on "init" you need to initialize your instance
bool ExtraGameOverScene::init()
{
	if (!Layer::init())
	{
		return false;
	}

    return true;
}

void ExtraGameOverScene::initHighScore()
{
	auto bgcolor = LayerColor::create(GameInfos[game].color, DESIGN_X, DESIGN_Y);
	bgcolor->setPosition(Point::ZERO);
	addChild(bgcolor);

#if !defined(LIGHT_THEME)
	addChild(BackgroundWidget::createWithoutBackground());
	//layer->addChild(Vignette::create());
#endif

	bool isHighscore = score >= highScore;
	
    AdsEnabled = true;
	if (!isHighscore) ShowFullscreenAd();
    
#if IS_IOS
    GameSettings::FetchGameCenterData(game);
#endif
    
	auto timeout = Label::createWithTTF(LS("GameOver"), BOLD_TTF, 46.0f);
	timeout->setHorizontalAlignment(TextHAlignment::CENTER);
	timeout->setAnchorPoint(Point(0.5f, 1.0f));
	timeout->setPosition(CP(0.5f, 0.6f));
	timeout->setColor(Color3B::WHITE);
	timeout->setTag(11);
	addChild(timeout);

	string s = string_format(LS("TXT_HIGHSCORECOLON"), highScore);
	if (score >= highScore)
	{
		s = LS("TXT_BROKEHIGHSCORE");
	}

	auto scorelbl = Label::createWithTTF(s, NORMAL_TTF, 42.0f);
	scorelbl->setHorizontalAlignment(TextHAlignment::CENTER);
	scorelbl->setAnchorPoint(Point(0.5f, 1.0f));
	scorelbl->setPosition(CP(0.5f, 0.5f));
	scorelbl->setColor(Color3B::WHITE);
	scorelbl->setTag(12);
	addChild(scorelbl);


	auto scorelbl2 = Label::createWithTTF(string_format(LS("TXT_YOURSCORECOLON"), score), NORMAL_TTF, 42.0f);
	scorelbl2->setHorizontalAlignment(TextHAlignment::CENTER);
	scorelbl2->setAnchorPoint(Point(0.5f, 1.0f));
	scorelbl2->setPosition(CP(0.5f, 0.43f));
	scorelbl2->setColor(Color3B::WHITE);
	scorelbl2->setTag(13);
	addChild(scorelbl2);

	float delay = 0.5f;
	for (int i = 11; i <= 13; i++, delay += 0.5f)
	{
		getChildByTag(i)->setOpacity(0);
		getChildByTag(i)->runAction(Sequence::createWithTwoActions(DelayTime::create(delay),
			FadeIn::create(0.25f)));
	}

#if defined(LOCAL_NAMEBOX)
	vector<Node*> things{ timeout }; //, scorelbl, scorelbl2};
	for (auto thing : things) thing->setPositionY(thing->getPositionY() + PY(0.25f));

	auto namelbl = Label::createWithTTF(LS("TXT_ENTERALIAS"), NORMAL_TTF, 42.0f);
	namelbl->setPosition(CP(0.5f, 0.68f));
	namelbl->setAnchorPoint(Point(0.5f, 0.0f));
	addChild(namelbl);

	nameBox = TextField::create(Ooze::Alias, NORMAL_TTF, 40.0f);
	nameBox->setPosition(CP(0.5f, 0.66f));
	nameBox->setAnchorPoint(Point(0.5f, 1.0f));
	nameBox->setTextColor(Color4B::WHITE);
	addChild(nameBox);

	auto menu = Menu::create();
	menu->setPosition(Point::ZERO);
	menu->setAnchorPoint(Point::ZERO);
	addChild(menu);

	auto btnSubmit = MakeButton("btnBlueUp", "btnBlueDn", LS("BTN_SUBMITSCORE"), 42.0f,
		CC_CALLBACK_1(ExtraGameOverScene::menuCloseCallback, this));
	btnSubmit->setPosition(VCP(0.5f, 0.3f));
	menu->addChild(btnSubmit);
#elif defined(DISABLE_SUBMIT) && !IS_IOS
	vector<Node*> things{ timeout, scorelbl, scorelbl2 };
	for (auto thing : things) thing->setPositionY(thing->getPositionY() + PY(0.25f));

	auto namelbl = Label::createWithTTF(LS("TXT_DISABLE_SUBMIT"), NORMAL_TTF, 38.0f);
	namelbl->setPosition(CP(0.5f, 0.5f));
	namelbl->setAnchorPoint(Point(0.5f, 0.5f));
	namelbl->setHorizontalAlignment(TextHAlignment::CENTER);
	namelbl->runAction(RepeatForever::create(Sequence::createWithTwoActions(ScaleTo::create(0.5f, 1.01f),
		ScaleTo::create(0.5f, 0.95f))));
	addChild(namelbl);

	auto menu = Menu::create();
	menu->setPosition(Point::ZERO);
	menu->setAnchorPoint(Point::ZERO);
	addChild(menu);

	auto btnSubmit = MakeButton("btnBlueUp", "btnBlueDn", LS("BTN_DISABLE_SUBMIT"), 42.0f,
		[](Ref*){BuyPro();});
	btnSubmit->setPosition(VCP(0.5f, 0.3f));
	menu->addChild(btnSubmit);
#else
	auto touch = EventListenerTouchOneByOne::create();
	touch->onTouchBegan = CC_CALLBACK_2(ExtraGameOverScene::onTouchBegan, this);
	getEventDispatcher()->addEventListenerWithSceneGraphPriority(touch, this);
#endif

	//scheduleOnce(schedule_selector(ExtraGameOverScene::advanceScreen), 4.0f);

	Node* facebookShareButton = FacebookShareButton::create(isHighscore, score, game);
	facebookShareButton->setPosition(CP(0.5f, -0.5f));
	facebookShareButton->runAction(MoveTo::create(1.0f, CP(0.5f, 0.1f)));
	facebookShareButton->setAnchorPoint(Point(0.5f, 0));
	addChild(facebookShareButton);

	PlayEffect("gameover.wav");

#ifdef LIGHT_THEME
	timeout->setColor(Color3B::BLACK);
	scorelbl->setColor(Color3B::BLACK);
	scorelbl2->setColor(Color3B::BLACK);
#if defined(LOCAL_NAMEBOX)
	namelbl->setColor(Color3B::BLACK);
	nameBox->setTextColor(Color4B::BLACK);
#endif
    /*recursiveAction(this, [](Node* n){
		if (dynamic_cast<LayerColor*>(n)) return;
        n->setColor(Color3B::BLACK);
    }, false);*/
#endif
    
	auto keyboard = EventListenerKeyboard::create();
	keyboard->onKeyReleased = CC_CALLBACK_2(ExtraGameOverScene::onKeyReleased, this);
	getEventDispatcher()->addEventListenerWithSceneGraphPriority(keyboard, this);

}


void ExtraGameOverScene::menuCloseCallback(Ref* pSender)
{
#if defined(LOCAL_NAMEBOX)
	if (nameBox->getString() != "")
	{
		Ooze::Alias = nameBox->getString();
		Ooze::SaveSettings();
	}
	GameSettings::ReportScore(game, score);
#endif
	advanceScreen(0);
}

bool ExtraGameOverScene::onTouchBegan(Touch*, Event*)
{
	unscheduleAllSelectors();
	advanceScreen();
	return true;
}

void ExtraGameOverScene::advanceScreen(float)
{
    GameSettings::TotalMenus++;
    GameSettings::SaveSettings();
    
    if (GameSettings::TotalMenus == 4) OpenRatePopup();
    log(string_format("Died %d times.", GameSettings::TotalMenus));
    
	Director::getInstance()->popScene();
	if (End) End();
}

void ExtraGameOverScene::onKeyReleased(EventKeyboard::KeyCode keyCode, Event* event)
{
	if (keyCode == EventKeyboard::KeyCode::KEY_ESCAPE)
	{
		unscheduleAllSelectors();
		advanceScreen();
	}
}