#include "ExtraLockedScene.h"
#include "ChooserScene.h"
#include "EpisodeChooserScene.h"
#include "BackgroundWidget.h"
#include "GameSettings.h"
#include "BundleGamesScene.h"
#include "Widgets.h"

Scene* ExtraLockedScene::createScene(Games game)
{
    auto scene = Scene::create();
	auto layer = ExtraLockedScene::create();
	layer->game = game;
	layer->createGamePanel();
    scene->addChild(layer);
    return scene;
}

bool ExtraLockedScene::init()
{
    if ( !Layer::init() )
    {
        return false;
    }
    
	auto keyboard = EventListenerKeyboard::create();
	keyboard->onKeyReleased = CC_CALLBACK_2(ExtraLockedScene::onKeyReleased, this);
	getEventDispatcher()->addEventListenerWithSceneGraphPriority(keyboard, this);

    oldGems = GameSettings::Gems;
    schedule(schedule_selector(ExtraLockedScene::checkGemsChanged), 1.0f);
    
    return true;
}

void ExtraLockedScene::checkGemsChanged(float)
{
    if (GameSettings::GetGems() > oldGems)
        Director::getInstance()->replaceScene(TransitionFade::create(0.5f, ExtraLockedScene::createScene(game)));
}

void ExtraLockedScene::createGamePanel()
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
	auto btnMenu = MAKEBUTTON("btnBackUp", "btnBackDn", ExtraLockedScene::menuCloseCallback);
	btnMenu->setPosition(VCP(0.03f, 0.95f));
	btnMenu->setAnchorPoint(Point(0, 1.0f));
	btnMenu->setScale(1.3f);
	hookerMenu->addChild(btnMenu);
#endif
    
	auto gems = GemsLayer::create();
	gems->setAnchorPoint(Point(1, 1));
	gems->setPosition(VCP(0.97f, 0.95f));
	hooker->addChild(gems);

	auto pMenu = Menu::create();
	pMenu->setPosition(Point::ZERO);
	addChild(pMenu, 1);

	auto episodetitle = Label::createWithTTF(GameInfos[game].title, NORMAL_TTF, 60.0f);
	episodetitle->setHorizontalAlignment(TextHAlignment::CENTER);
	episodetitle->setPosition(CP(0.5f, 0.45f));
	addChild(episodetitle);

	string extraLockedGameMessage = string_format(LS("ExtraLockedGameMessage"), 
		GameInfos[game].price);
	auto unlocktext = Label::createWithTTF(extraLockedGameMessage, NORMAL_TTF, 38.0f);
	unlocktext->setHorizontalAlignment(TextHAlignment::CENTER);
	unlocktext->setPosition(CP(0.5f, 0.30f));
	addChild(unlocktext);

	if (GameSettings::Gems >= GameInfos[game].price)
	{
		//show buy button
		auto btnBuy = MakeButton("btnGreenUp", "btnGreenDn", LS("UnlockGame") /* + Games[game].title*/, 42.0f,
			CC_CALLBACK_1(ExtraLockedScene::btnBuyPressed, this));
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

}

void ExtraLockedScene::onKeyReleased(EventKeyboard::KeyCode keyCode, Event* event)
{
	if (keyCode == EventKeyboard::KeyCode::KEY_ESCAPE)
		menuCloseCallback(this);
}

void ExtraLockedScene::menuCloseCallback(Ref* pSender)
{
	Director::getInstance()->replaceScene(TransitionFlipY::create(1.0f, BundleGamesScene::createScene()));
}

void ExtraLockedScene::btnBuyPressed(Ref*)
{
	PlayEffect("buy.wav");
	if (GameSettings::SpendGems(GameInfos[game].price))
	{
		if (game == Games::Mesh) GameSettings::MeshUnlocked = true;
		else if (game == Games::Tree) GameSettings::TreeUnlocked = true;
		else if (game == Games::Snake) GameSettings::SnakeUnlocked = true;
		else if (game == Games::Raid) GameSettings::RaidUnlocked = true;
		else if (game == Games::Fall) GameSettings::FallUnlocked = true;

#if IS_IOS
		ShowPopup("Game Unlocked", "Congratulations! You have unlocked " + GameInfos[game].title + " for " +
			nToString(GameInfos[game].price) + " gems!");
#endif
	}
    unschedule(schedule_selector(ExtraLockedScene::checkGemsChanged));

	Director::getInstance()->replaceScene(TransitionFlipY::create(1.0f, BundleGamesScene::createScene()));
}