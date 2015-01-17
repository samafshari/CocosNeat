#include "GameInfoScene.h"
#include "ChooserScene.h"
#include "EpisodeChooserScene.h"
#include "BackgroundWidget.h"
#include "GameSettings.h"
#include "BundleGamesScene.h"
#include "Widgets.h"

Scene* GameInfoScene::createScene(Games game)
{
    auto scene = Scene::create();
	auto layer = GameInfoScene::create();
	layer->game = game;
	layer->createGamePanel();
    scene->addChild(layer);
    return scene;
}

bool GameInfoScene::init()
{
    if ( !Layer::init() )
    {
        return false;
    }
    
	auto keyboard = EventListenerKeyboard::create();
	keyboard->onKeyReleased = CC_CALLBACK_2(GameInfoScene::onKeyReleased, this);
	getEventDispatcher()->addEventListenerWithSceneGraphPriority(keyboard, this);
	
    oldGems = GameSettings::Gems;
    schedule(schedule_selector(GameInfoScene::checkGemsChanged), 1.0f);
    
    return true;
}

void GameInfoScene::checkGemsChanged(float)
{
    if (GameSettings::GetGems() > oldGems)
        Director::getInstance()->replaceScene(TransitionFade::create(0.5f, GameInfoScene::createScene(game)));
}

void GameInfoScene::createGamePanel()
{
	Size visibleSize = Director::getInstance()->getVisibleSize();
	auto bgcolor = LayerColor::create(GameInfos[game].color, visibleSize.width, visibleSize.height);
	bgcolor->setPosition(Point::ZERO);
	addChild(bgcolor);

	addChild(BackgroundWidget::createWithoutBackground());
    
	auto hooker = TopAnchor::create();
	addChild(hooker, 1);
    
	addChild(Vignette::create());

	auto gamelogo = MAKESPRITE(GameInfos[game].logo);
	hooker->addChild(gamelogo);
	gamelogo->setPosition(VCP(0.5f, 0.70f));
	gamelogo->runAction(RepeatForever::create(Sequence::createWithTwoActions(
		ScaleTo::create(3.0f, 1.05f),
		ScaleTo::create(3.0f, 1.0f)
		)));

	auto hookerMenu = Menu::create();
	hookerMenu->setPosition(Point::ZERO);
	hooker->addChild(hookerMenu);
    
#if (CC_TARGET_PLATFORM != CC_PLATFORM_ANDROID)
	auto btnMenu = MAKEBUTTON("btnBackUp", "btnBackDn", GameInfoScene::menuCloseCallback);
	btnMenu->setPosition(VCP(0.03f, 0.95f));
	btnMenu->setAnchorPoint(Point(0, 1.0f));
	btnMenu->setScale(1.3f);
	hookerMenu->addChild(btnMenu);
    
	auto gems = GemsLayer::create();
	gems->setAnchorPoint(Point(1, 1));
	gems->setPosition(VCP(0.97f, 0.95f));
	hooker->addChild(gems);
#endif
    
	auto pMenu = Menu::create();
	pMenu->setPosition(Point::ZERO);
	addChild(pMenu, 1);

	auto episodetitle = Label::createWithTTF(LS(GameInfos[game].title), NORMAL_TTF, 60.0f);
	episodetitle->setHorizontalAlignment(TextHAlignment::CENTER);
	episodetitle->setPosition(CP(0.5f, 0.45f));
	addChild(episodetitle);

	auto unlocktext = Label::createWithTTF(LS(GameInfos[game].description), NORMAL_TTF, 38.0f);
	unlocktext->setHorizontalAlignment(TextHAlignment::CENTER);
	unlocktext->setPosition(CP(0.5f, 0.30f));
	addChild(unlocktext);

#if (CC_TARGET_PLATFORM != CC_PLATFORM_ANDROID)
	if (GameSettings::Gems >= 1)
	{
		//show buy button
		auto btnBuy = MakeButton("btnGreenUp", "btnGreenDn", LS("PlayFor1Gem"), 42.0f,
			CC_CALLBACK_1(GameInfoScene::btnBuyPressed, this));
		btnBuy->setPosition(CP(0.5f, 0.05f));
		btnBuy->setAnchorPoint(Point(0.5f, 0));
		pMenu->addChild(btnBuy);
	}
	else
	{
		auto errortext = Label::createWithTTF(LS("NotEnoughGems"), FANCY_TTF, 42.0f);
		errortext->setColor(Color3B::RED);
		errortext->enableShadow();
		errortext->setPosition(CP(0.5f, 0.05f));
		addChild(errortext);
	}
#else
    auto btnBuy = MakeButton("btnGreenUp", "btnGreenDn", LS("Play"), 42.0f,
                             CC_CALLBACK_1(GameInfoScene::btnBuyPressed, this));
    btnBuy->setPosition(CP(0.5f, 0.05f));
    btnBuy->setAnchorPoint(Point(0.5f, 0));
    pMenu->addChild(btnBuy);
#endif
}

void GameInfoScene::onKeyReleased(EventKeyboard::KeyCode keyCode, Event* event)
{
	if (keyCode == EventKeyboard::KeyCode::KEY_ESCAPE)
		menuCloseCallback(this);
}

void GameInfoScene::menuCloseCallback(Ref* pSender)
{
	Director::getInstance()->replaceScene(TransitionFlipX::create(1.0f, BundleGamesScene::createScene()));
}

void GameInfoScene::btnBuyPressed(Ref*)
{
    BundleGamesScene::RunGame(game);
}