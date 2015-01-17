#include "EpisodeLockedScene.h"
#include "ChooserScene.h"
#include "EpisodeChooserScene.h"
#include "BackgroundWidget.h"
#include "Widgets.h"

Scene* EpisodeLockedScene::createScene()
{
    auto scene = Scene::create();
	auto layer = EpisodeLockedScene::create();
    scene->addChild(layer);
    return scene;
}

bool EpisodeLockedScene::init()
{
    if ( !Layer::init() )
    {
        return false;
    }
    
    Size visibleSize = Director::getInstance()->getVisibleSize();
	
	addChild(BackgroundWidget::create("bg" + nToString(GameSettings::CurrentEpisode),
                                      Color3B(200, 200, 200)));

	addChild(Vignette::create());

	auto hole = MAKESPRITE("blackholebig");
	addChild(hole);
	hole->setPosition(CP(0.5f, 0.65f));
	hole->setScale(0.8f);
	hole->runAction(RepeatForever::create(RotateBy::create(40.0, 365)));

	auto gamelogo = MAKESPRITE("gamelogo");
	addChild(gamelogo);
	gamelogo->setPosition(CP(0.5f, 0.65f));
	gamelogo->setScale(0.8f);
	gamelogo->runAction(RepeatForever::create(Sequence::createWithTwoActions(
		ScaleTo::create(3.0f, 0.81f),
		ScaleTo::create(3.0f, 0.79f)
		)));

	string title = LS(EpisodeInfos[GameSettings::CurrentEpisode].title);
	int price = EpisodeInfos[GameSettings::CurrentEpisode].price;

    auto hooker = TopAnchor::create();
    addChild(hooker, 1);

	auto hookerMenu = Menu::create();
	hookerMenu->setPosition(Point::ZERO);
	hooker->addChild(hookerMenu);
    
#if (CC_TARGET_PLATFORM != CC_PLATFORM_ANDROID)
	auto btnMenu = MAKEBUTTON("btnBackUp", "btnBackDn", EpisodeLockedScene::menuCloseCallback);
	btnMenu->setPosition(VCP(0.03f, 0.95f));
	btnMenu->setAnchorPoint(Point(0, 1.0f));
	btnMenu->setScale(1.3f);
	hookerMenu->addChild(btnMenu);
#endif
	auto pMenu = Menu::create();
	pMenu->setPosition(Point::ZERO);
	addChild(pMenu, 1);

	auto episodetext = Label::createWithTTF(LS("Episode") + " " + 
		nToString(GameSettings::CurrentEpisode + 1), FANCY_TTF, 50.0f);
	episodetext->setHorizontalAlignment(TextHAlignment::CENTER);
	episodetext->setPosition(VCP(0.5f, 0.45f));
	addChild(episodetext);
	
	auto episodetitle = Label::createWithTTF(title, FANCY_TTF, 60.0f);
	episodetitle->setHorizontalAlignment(TextHAlignment::CENTER);
	episodetitle->setPosition(VCP(0.5f, 0.38f));
	addChild(episodetitle);

	string unlocktextstring = LS("UnlockEpisodeMessageWithProgress");
	
	auto unlocktext = Label::createWithTTF(string_format(unlocktextstring, price), FANCY_TTF, 38.0f);
	unlocktext->setHorizontalAlignment(TextHAlignment::CENTER);
	unlocktext->setPosition(VCP(0.5f, 0.25f));
	addChild(unlocktext);

	auto keyboard = EventListenerKeyboard::create();
	keyboard->onKeyReleased = CC_CALLBACK_2(EpisodeLockedScene::onKeyReleased, this);
	getEventDispatcher()->addEventListenerWithSceneGraphPriority(keyboard, this);

    return true;
}

void EpisodeLockedScene::checkGemsChanged(float)
{
	if (GameSettings::GetGems() > oldGems)
		Director::getInstance()->replaceScene(TransitionFade::create(0.1f, 
		EpisodeLockedScene::createScene()));
}

void EpisodeLockedScene::onKeyReleased(EventKeyboard::KeyCode keyCode, Event* event)
{
	if (keyCode == EventKeyboard::KeyCode::KEY_ESCAPE)
		menuCloseCallback(this);
}

void EpisodeLockedScene::menuCloseCallback(Ref* pSender)
{
	Director::getInstance()->replaceScene(TransitionFade::create(1.0f, EpisodeChooser::createScene()));
}

void EpisodeLockedScene::btnBuyPressed(Ref*)
{
	PlayEffect("buy.wav");
	//if (GameSettings::SpendGems(EpisodeInfos[GameSettings::CurrentEpisode].price))
	{
		GameSettings::UnlockedEpisodes.insert(GameSettings::CurrentEpisode);
		GameSettings::SaveSettings();

		unschedule(schedule_selector(EpisodeLockedScene::checkGemsChanged));
		Director::getInstance()->replaceScene(TransitionFade::create(1.0f, ChooserScene::scene()));
	}
}