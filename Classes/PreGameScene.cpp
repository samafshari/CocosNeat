#include "PreGameScene.h"
#include "helpers.h"
#include "MainMenuScene.h"
#include "DronesScene.h"
#include "ChooserScene.h"

Scene* PreGame::scene()
{
	Scene *scene = Scene::create();
	PreGame *layer = PreGame::create();
    scene->addChild(layer);
    return scene;
}

static float beginDelay = 1.3f;
static float fadeInTime = 1.1f;
static Point moveDist = CP(0.00f, 0.05f);

bool PreGame::init()
{
    if ( !Layer::init() )
    {
        return false;
    }

	Size visibleSize = Director::getInstance()->getVisibleSize();

	LayerColor* bgcolor = LayerColor::create(C4Fto4B(0.0f, 0.0f, 0.0f, 1.0f), 
		visibleSize.width, visibleSize.height);
	this->addChild(bgcolor);

	Label* lbl = Label::createWithTTF(LS("NowEntering"), NORMAL_TTF, 32.0);
	lbl->setColor(Color3B(255, 255, 255));
	lbl->setPosition(CP(0.5f, 0.5f));
	lbl->runAction(FadeIn::create(fadeInTime));
	lbl->runAction(MoveBy::create(GameSettings::CurrentLevelId > 0 ? 0.01f : fadeInTime, moveDist));
	addChild(lbl);
	lbl->setOpacity(0);

	string title = LS("Level") + " " + nToString(GameSettings::CurrentLevelId + 1);
	Label* pl2 = Label::createWithTTF(title, NORMAL_TTF, 52.0);
	pl2->setPosition(CP(0.5f, 0.42f));
	pl2->setColor(Color3B(255, 255, 255));
	pl2->runAction(Sequence::createWithTwoActions(MoveBy::create(beginDelay, Point::ZERO), FadeIn::create(fadeInTime)));
	pl2->runAction(Sequence::createWithTwoActions(MoveBy::create(beginDelay, Point::ZERO), MoveBy::create(fadeInTime, moveDist)));
	addChild(pl2);
	pl2->setOpacity(0);

	auto keyboard = EventListenerKeyboard::create();
	keyboard->onKeyReleased = CC_CALLBACK_2(PreGame::onKeyReleased, this);
	getEventDispatcher()->addEventListenerWithSceneGraphPriority(keyboard, this);

	this->scheduleOnce(schedule_selector(PreGame::advance), 3.5f);

    return true;
}

void PreGame::advance(float dt)
{
	Director::getInstance()->replaceScene(TransitionFade::create(1.0f, Drones::DronesScene::scene()));
}

void PreGame::onKeyReleased(EventKeyboard::KeyCode keyCode, Event*)
{
	if (keyCode == EventKeyboard::KeyCode::KEY_ESCAPE)
		Director::getInstance()->replaceScene(TransitionFade::create(1.0f, MainMenu::scene()));
}