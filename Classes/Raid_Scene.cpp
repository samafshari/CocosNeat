#include "Raid.h"
#include "ExtraGameOverScene.h"
#include "Ooze.h"
#include "RaidSwipeController.h"

using namespace Raid;

int RaidScene::Level = 0;

Scene* RaidScene::scene()
{
    GameSettings::RaidPlayed++;
    GameSettings::ReportAchievement(ACHIEVEMENT_UNLOCKED_RAID);
    GameSettings::TryGiveUnlockAll();
    GameSettings::SaveSettings();
    Ooze::InstantSend({{"Event", "GameStart"}, {"Game", "Raid"}});
    GameSettings::OozeGameStats();
	Scene *scene = Scene::create();
	RaidScene *layer = RaidScene::create();
	scene->addChild(layer);
	return scene;
}

bool RaidScene::init()
{
	if (!Layer::init()) { return false; }

	auto keyboard = EventListenerKeyboard::create();
	keyboard->onKeyReleased = CC_CALLBACK_2(RaidScene::onKeyReleased, this);
	getEventDispatcher()->addEventListenerWithSceneGraphPriority(keyboard, this);
/*
	auto tilt = EventListenerAcceleration::create(CC_CALLBACK_2(RaidScene::onAcceleration, this));
	getEventDispatcher()->addEventListenerWithSceneGraphPriority(tilt, this);
*/
	addChild(SwipeController::create());

	auto touchListener = EventListenerTouchOneByOne::create();
	touchListener->onTouchBegan = CC_CALLBACK_2(RaidScene::onTouchBegan, this);
	touchListener->onTouchEnded = CC_CALLBACK_2(RaidScene::onTouchEnded, this);
	touchListener->onTouchCancelled = CC_CALLBACK_2(RaidScene::onTouchEnded, this);
	getEventDispatcher()->addEventListenerWithSceneGraphPriority(touchListener, this);
	
    auto hooker = TopAnchor::create();
    addChild(hooker, 1);
	Menu* pMenu = Menu::create();
	pMenu->setPosition(Point::ZERO);
	hooker->addChild(pMenu, 1);

	game = RaidLayer::create();
	game->setTag(TAG_RAIDLAYER);
	addChild(game);

#if (CC_TARGET_PLATFORM != CC_PLATFORM_ANDROID)
	auto btnBack = MAKEBUTTON("btnMenuUp", "btnMenuDn", btnBackPressed);
	pMenu->addChild(btnBack);
	btnBack->setPosition(VCP(0.05f, 1.0f));
    btnBack->setAnchorPoint(Point(-0.5f, 1.5f));
#endif
    
    string sprite = GameSettings::MuteSounds ? "btnMute" : "btnSound";
	string psprite = sprite + "Dn";
	sprite += "Up";
	
    btnSound = MAKEBUTTON(sprite, psprite, btnSoundPressed);
	pMenu->addChild(btnSound);
	btnSound->setPosition(VCP(0.95f, 1.0f));
	btnSound->setAnchorPoint(Point(1.5f, 1.5f));

	schedule(schedule_selector(RaidScene::shoot), 0.3f);

	return true;
}

void RaidScene::btnSoundPressed(Ref*)
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

bool RaidScene::onTouchBegan(Touch* touch, Event*)
{
	if (touch->getLocation().y > PY(0.75f)) return false;
	//shoot();
	//schedule(schedule_selector(TouchController::shoot));
	return true;// move(touch);
}


void RaidScene::onTouchEnded(Touch* touch, Event*)
{
	//unschedule(schedule_selector(RaidScene::shoot));
}

void RaidScene::shoot(float)
{
	shoot();
}

bool RaidScene::move(Touch* touch)
{
	move(CC_RADIANS_TO_DEGREES(
		(touch->getLocation() - CP(0.5f, 0.1f)).getAngle()),
		fabs(touch->getLocation().x - PX(0.5f)) / (DESIGN_X * 0.5f));

	return true;
}

void RaidScene::onAcceleration(Acceleration* accel, Event*)
{
	if (accel->x < -0.05f) move(180, fabs(accel->x) * 20.0f); //(Directions::Left);
	else if (accel->x > 0.05f) move(0, fabs(accel->x) * 20.0f); //(Directions::Right);
	else move(90);  //(Directions::Stop);

	boost(-accel->y - 0.7f);
}

void RaidScene::btnBackPressed(Ref*)
{
	gameOver(true);
}

void RaidScene::onKeyReleased(EventKeyboard::KeyCode keyCode, Event*)
{
	if (keyCode == EventKeyboard::KeyCode::KEY_ESCAPE)
		btnBackPressed(this);
	if (keyCode == EventKeyboard::KeyCode::KEY_RIGHT_ARROW)
		move(0, 4);
	if (keyCode == EventKeyboard::KeyCode::KEY_LEFT_ARROW)
		move(180, 4);
	if (keyCode == EventKeyboard::KeyCode::KEY_UP_ARROW)
		move(90);
	if (keyCode == EventKeyboard::KeyCode::KEY_SPACE)
		shoot();
	if (keyCode == EventKeyboard::KeyCode::KEY_R)
		Director::getInstance()->replaceScene(RaidScene::scene());
}

void RaidScene::move(float angle, float radius)
{
	game->move(angle, radius);
}

void RaidScene::boost(float tilt)
{
	game->setBoost(tilt);
}

void RaidScene::shoot()
{
	game->shoot();
}

void RaidScene::gameOver(bool backPressed)
{
	GameSettings::RaidLastScore = game->score;
	if (GameSettings::RaidHighScore <= GameSettings::RaidLastScore)
		GameSettings::RaidHighScore = GameSettings::RaidLastScore;
	GameSettings::RaidTotalScore += GameSettings::RaidLastScore;

    if (!backPressed && game->score == 0)
        GameSettings::ReportAchievement(ACHIEVEMENT_LOSE_RAID);
	GameSettings::SaveSettings();
    GameSettings::ReportScore(LEADERBOARD_RAID_LEVEL, Level);
	unscheduleAllSelectors();

	Director::getInstance()->replaceScene(TransitionFade::create(1.0f, 
		ExtraGameOverScene::createScene(Games::Raid)));
}

void RaidScene::teleport(float x)
{
	game->teleport(x);
}