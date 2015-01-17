#include "Raid.h"
#include "CCScale9Sprite.h"
//#include "extensions/cocos-ext.h"
#include "ExtraGameOverScene.h"
#include "Ooze.h"

#ifdef FA
const float VALUESCALE = 1.0f;
#else
float VALUESCALE = 0.7f;
#endif

using namespace Raid;

RaidLayer* RaidLayer::CurrentGame = nullptr;

bool RaidLayer::init()
{
	if (!Layer::init()) { return false; }

	createGame();

	winStatsLayer = Label::createWithTTF(" ", NORMAL_TTF, 48.0f);
	winStatsLayer->setHorizontalAlignment(TextHAlignment::CENTER);
	winStatsLayer->setAnchorPoint(Point(0.5f, 1.0f));
	winStatsLayer->setPosition(Point(PX(0.5f), PY(-0.5f)));
	winStatsLayer->setColor(Color3B::WHITE);
	winStatsLayer->setTag(12);
	addChild(winStatsLayer);

	RaidLayer::CurrentGame = this;
	return true;
}

void RaidLayer::createMenu()
{
	menu = Menu::create();
	menu->setTag(kLYR_MENU);
	addChild(menu, 2000);
	
	auto btnLaser = MAKEBUTTON("btnLaserUp", "btnLaserOn", RaidLayer::switchWeaponPressed);
	btnLaser->setTag(kBTN_LASER);
	btnLaser->setAnchorPoint(Point(0.5f, 1.0f));
	btnLaser->setPosition(CP(0.3f, 0));
	menu->addChild(btnLaser);

	auto btnRocket = MAKEBUTTON("btnRocketUp", "btnRocketOn", RaidLayer::switchWeaponPressed);
	btnRocket->setTag(kBTN_ROCKET);
	btnRocket->setAnchorPoint(Point(0.5f, 1.0f));
	btnRocket->setPosition(CP(0.5f, 0));
	menu->addChild(btnRocket);

	auto btnGun = MAKEBUTTON("btnGunUp", "btnGunOn", RaidLayer::switchWeaponPressed);
	btnGun->setTag(kBTN_GUN);
	btnGun->setAnchorPoint(Point(0.5f, 1.0f));
	btnGun->setPosition(CP(0.7f, 0));
	menu->addChild(btnGun);

	menu->setPosition(Point(0, VPY(0.97f)));
	switchWeaponPressed(btnLaser);

#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)
	menu->setPositionX(-PX(0.1f));
#endif
}

void RaidLayer::switchWeaponPressed(Ref* r)
{
	Node* n = static_cast<Node*>(r);
	log("SWITCH: " + nToString(n->getTag()));
	selectedWeapon = n->getTag();

	if (n->getTag() == kBTN_LASER)
		static_cast<MenuItemSprite*>(menu->getChildByTag(kBTN_LASER))->setNormalImage(MAKESPRITE("btnLaserOn"));
	else
		static_cast<MenuItemSprite*>(menu->getChildByTag(kBTN_LASER))->setNormalImage(MAKESPRITE("btnLaserUp"));

	if (n->getTag() == kBTN_ROCKET)
		static_cast<MenuItemSprite*>(menu->getChildByTag(kBTN_ROCKET))->setNormalImage(MAKESPRITE("btnRocketOn"));
	else
		static_cast<MenuItemSprite*>(menu->getChildByTag(kBTN_ROCKET))->setNormalImage(MAKESPRITE("btnRocketUp"));

	if (n->getTag() == kBTN_GUN)
		static_cast<MenuItemSprite*>(menu->getChildByTag(kBTN_GUN))->setNormalImage(MAKESPRITE("btnGunOn"));
	else
		static_cast<MenuItemSprite*>(menu->getChildByTag(kBTN_GUN))->setNormalImage(MAKESPRITE("btnGunUp"));

	shoot();
}

void RaidLayer::createGame()
{
	canvas = Layer::create();// ShaderLayer::create("game.glsl");
	canvas->setTag(kLYR_CANVAS);
	addChild(canvas);

	rockets = Node::create();
	canvas->addChild(rockets);

	_innerWallTop = 0;
	_outerWallTop = 0;

	RaidScene::Level = 0;

	gameLayer = Layer::create();
	canvas->addChild(gameLayer);

	auto vig = Sprite::create("vig.png");
	auto vigSize = vig->getContentSize();
	vig->setScaleX(DESIGN_X / vigSize.width);
	vig->setScaleY(DESIGN_Y / vigSize.height);
	vig->setPosition(CP(0.5f, 0.5f));
	canvas->addChild(vig);

	auto redvig = Sprite::create("redvig.png");
	auto redvigSize = redvig->getContentSize();
	redvig->setScaleX(DESIGN_X / redvigSize.width);
	redvig->setScaleY(DESIGN_Y / redvigSize.height);
	redvig->setOpacity(0);
	redvig->setPosition(CP(0.5f, 0.5f));
	redvig->setTag(kIMG_REDVIG);
	canvas->addChild(redvig);

	levels = Layer::create();
	gameLayer->addChild(levels);

	player = Entity::create("raider");
	player->setAnchorPoint(Point(0.5f, 0));
	canvas->addChild(player);

	schedule(schedule_selector(RaidLayer::updateGame), 0);

	auto fuelDashboard = MAKESPRITE("dashboard");
	fuelDashboard->setTag(kFUEL_DASHBOARD);
	fuelDashboard->setAnchorPoint(Point(0.0f, 0.0f));
	dashboardSize = fuelDashboard->getContentSize();
	fuelDashboard->setPosition(Point(PX(0.5f) - dashboardSize.width * 0.5f,
		PY(0.0f)));
	auto dashboardbg = LayerColor::create(Color4B::WHITE, DESIGN_X,
		dashboardSize.height);
	addChild(dashboardbg, 1999);
	addChild(fuelDashboard, 2000);

	auto fuelguage = LayerColor::create(Color4B::GREEN, dashboardSize.width * 0.04f,
		dashboardSize.height * 0.25f);
	fuelguage->setTag(kFUEL_GAUGE);
	fuelguage->ignoreAnchorPointForPosition(false);
	fuelDashboard->addChild(fuelguage, -1);
	fuelguage->setAnchorPoint(Point(1.0f, 1.0f));
	fuelguage->setPositionX(dashboardSize.width);
	fuelguage->setPositionY(dashboardSize.height);

	fuelDashboard->setOpacity(0);
	fuelguage->setOpacity(0);

	Size visibleSize = Director::getInstance()->getVisibleSize();
	auto bg = LayerColor::create(Color4B(50, 50, 200, 255), visibleSize.width, visibleSize.height);
	addChild(bg, -10);

	auto lyrScore = Layer::create();
	lyrScore->setTag(kLYR_SCORE);
	addChild(lyrScore, 2000);
	scoreActive = true;

	auto lyrLevel = Layer::create();
	lyrLevel->setTag(kLYR_LEVEL);
	//lyrLevel->setPositionX(PX(0.5f));
	addChild(lyrLevel, 2000);

	const float dashboardLabelFontSize = 40.0f;
	
#ifdef FA
	const float dashboardLabelAnchorY = 0.1f;
	const float dashboardValueAnchorY = 0.70f;
#else
	const float dashboardLabelAnchorY = -0.01f;
	const float dashboardValueAnchorY = 0.80f;
#endif
	auto lblScore = Label::createWithTTF(LS("Score"), NORMAL_TTF, dashboardLabelFontSize);
	lblScore->setAnchorPoint(Point(0.5f, dashboardLabelAnchorY));
	lblScore->setHorizontalAlignment(TextHAlignment::CENTER);
	lblScore->setScale(0.7f);
	lblScore->setColor(Color3B::BLACK);
	lblScore->setPosition(Point(PX(0.7f), dashboardSize.height * 0.5f));
	lyrScore->addChild(lblScore, 2000);

	auto lblLevel = Label::createWithTTF(LS("Level"), NORMAL_TTF, dashboardLabelFontSize);
	lblLevel->setAnchorPoint(Point(1, dashboardLabelAnchorY));
	lblLevel->setHorizontalAlignment(TextHAlignment::RIGHT);
	lblLevel->setScale(0.7f);
	lblLevel->setColor(Color3B::BLACK);
	lblLevel->setPosition(Point(PX(0.98f), dashboardSize.height * 0.5f));
	lyrLevel->addChild(lblLevel, 2000);

	auto lblLives = Label::createWithTTF(LS("RAID_Lives"), NORMAL_TTF, dashboardLabelFontSize);
	lblLives->setAnchorPoint(Point(0, dashboardLabelAnchorY));
	lblLives->setHorizontalAlignment(TextHAlignment::LEFT);
	lblLives->setScale(0.7f);
	lblLives->setColor(Color3B::BLACK);
	lblLives->setPosition(Point(PX(0.02), dashboardSize.height * 0.5f));
	addChild(lblLives, 2000);

	auto lblSpeed = Label::createWithTTF(LS("Speed"), NORMAL_TTF, dashboardLabelFontSize);
	lblSpeed->setAnchorPoint(Point(0.5f, dashboardLabelAnchorY));
	lblSpeed->setHorizontalAlignment(TextHAlignment::CENTER);
	lblSpeed->setScale(0.7f);
	lblSpeed->setColor(Color3B::BLACK);
	lblSpeed->setPosition(Point(PX(0.4f), dashboardSize.height * 0.5f));
	addChild(lblSpeed, 2000);

	auto txtScore = Label::createWithBMFont(NORMAL_FNT, "0");
	txtScore->setScale(VALUESCALE);
	txtScore->setAnchorPoint(Point(0.5f, dashboardValueAnchorY));
	txtScore->setHorizontalAlignment(TextHAlignment::CENTER);
	txtScore->setColor(Color3B::BLACK);
	txtScore->setPosition(Point(PX(0.7f), dashboardSize.height * 0.5f));
	txtScore->setTag(kTXT_SCORE);
	lyrScore->addChild(txtScore, 2000);

	auto txtLevel = Label::createWithBMFont(NORMAL_FNT, nToString(1 + RaidScene::Level));
	txtLevel->setScale(VALUESCALE);
	txtLevel->setAnchorPoint(Point(1, dashboardValueAnchorY));
	txtLevel->setHorizontalAlignment(TextHAlignment::RIGHT);
	txtLevel->setColor(Color3B::BLACK);
	txtLevel->setPosition(Point(PX(0.98f), dashboardSize.height * 0.5f));
	txtLevel->setTag(kTXT_LEVEL);
	lyrLevel->addChild(txtLevel, 2000);

	auto txtSpeed = Label::createWithBMFont(NORMAL_FNT, "100%");
	txtSpeed->setScale(VALUESCALE);
	txtSpeed->setAnchorPoint(Point(0.5f, dashboardValueAnchorY));
	txtSpeed->setHorizontalAlignment(TextHAlignment::CENTER);
	txtSpeed->setColor(Color3B::BLACK);
	txtSpeed->setPosition(Point(PX(0.4f), dashboardSize.height * 0.5f));
	txtSpeed->setTag(kTXT_SPEED);
	addChild(txtSpeed, 2000);

	auto lyrLives = Node::create();
	lyrLives->setPosition(Point(PX(0.02), dashboardSize.height * 0.5f));
	lyrLives->setTag(kLYR_LIVES);
	addChild(lyrLives, 2000);

	schedule(schedule_selector(RaidLayer::playFuelSound), 0.2f);
	lives = 3;
	score = 0;
	scoreInLife = 0;
	reloadGame();

	auto fuelBar = LayerColor::create(Color4B::WHITE, PX(0.05f), PY(1.0f) - dashboardSize.height);
	fuelBar->ignoreAnchorPointForPosition(false);
	auto speedBarSize = fuelBar->getContentSize();
	fuelBar->setAnchorPoint(Point(1, 0));
	fuelBar->setPosition(Point(PX(1.0f), dashboardSize.height));
	fuelBar->setTag(kLYR_SPEEDBAR);
	//fuelBar->setScale(0.25f);
	//fuelBar->setScaleY((VPY(1.0f) - dashboardSize.height) / speedBarSize.height);
	addChild(fuelBar, 1000);

	auto fuelGuage = LayerColor::create(Color4B::BLACK, speedBarSize.width, speedBarSize.height);
	fuelGuage->setTag(kLYR_SPEEDGUAGE);
	fuelGuage->setScaleY(0);
	fuelGuage->ignoreAnchorPointForPosition(false);
	fuelGuage->setAnchorPoint(Point::ZERO);
	fuelBar->addChild(fuelGuage);

	auto fuelLabel = Label::createWithTTF(LS("Fuel"), NORMAL_TTF, 28.0f);
	fuelLabel->setColor(Color3B::WHITE);
#ifdef FA
	fuelLabel->setRotation(270);
	fuelLabel->setAnchorPoint(Point(0.1f, 0.1f));
#else
	fuelLabel->setRotation(90);
	fuelLabel->setAnchorPoint(Point(-0.1f, 0.0f));
	fuelLabel->setScale(-1);
#endif
	fuelLabel->setTag(kLBL_SPEED);
	//fuelLabel->enableOutline(Color4B::BLACK, 1);
	fuelLabel->setPositionX(fuelBar->getPositionX());// -speedBarSize.width * fuelBar->getScaleX());
	fuelLabel->setPositionY(fuelBar->getPositionY());
	fuelLabel->setHorizontalAlignment(TextHAlignment::RIGHT);
	addChild(fuelLabel, 1001);

	updateFuelUI();

	auto lifeBar = LayerColor::create(Color4B::WHITE, PX(0.05f), PY(1.0f) - dashboardSize.height);
	lifeBar->ignoreAnchorPointForPosition(false);
	//Scale9Sprite::createWithSpriteFrameName("speedbar");
	lifeBar->setAnchorPoint(Point(0, 0));
	lifeBar->setPosition(Point(0, dashboardSize.height));
	lifeBar->setTag(kLYR_LIFEBAR);
	//lifeBar->setScale(0.25f);
	//lifeBar->setScaleY((VPY(1.0f) - dashboardSize.height) / speedBarSize.height);
	addChild(lifeBar, 1000);

	auto lifeGuage = LayerColor::create(Color4B::BLACK, speedBarSize.width, speedBarSize.height);
	lifeGuage->setTag(kLYR_LIFEGUAGE);
	lifeGuage->setScaleY(0);
	lifeGuage->ignoreAnchorPointForPosition(false);
	lifeGuage->setAnchorPoint(Point::ZERO);
	lifeBar->addChild(lifeGuage);

	auto lifeLabel = Label::createWithTTF(LS("ExtraLife"), NORMAL_TTF, 28.0f);
	lifeLabel->setColor(Color3B::WHITE);
	lifeLabel->setRotation(90);
	lifeLabel->setAnchorPoint(Point(-0.1f, 0));
	lifeLabel->setTag(kLBL_LIFE);
	lifeLabel->setPosition(lifeBar->getPosition());
	//lifeLabel->enableOutline(Color4B::BLACK, 2);
	addChild(lifeLabel, 1001);

	selectedWeapon = kBTN_NORMALLASER;
	//createMenu();
}

void RaidLayer::reloadGame()
{
	//if (state == GameStates::Intro) return;
	canvas->removeChildByTag(kPLAYER_EXPLOSION);

	state = GameStates::Intro;
	switchToLevelTxt();
	fuel = 1.0f;

	player->setOpacity(255);
	player->removeAllChildren();

	stopRocket();
	gameLayer->setPositionY(0);

	levels->removeAllChildren();

	loadLevel(-1);
	for (int i = 0; i < 2; i++) loadLevel(RaidScene::Level + i);

	_accel = 0;
	player->setPositionX(PX(0.5f));
	player->setPositionY(-PY(0.1f));
	player->setAnchorPoint(Point(0.5f, 0));
	player->setSpriteFrame("raider");
	player->setFlippedX(false);

	_boost = 1;

	_entityBoost = SPEED_BOOST_EASY;
	if (RaidScene::Level >= 4) _entityBoost = SPEED_BOOST_MEDIUM;
	if (RaidScene::Level >= 8) _entityBoost = SPEED_BOOST_HARD;
	if (RaidScene::Level >= 12) _entityBoost = SPEED_BOOST_BRUTAL;

	updateLivesUI();
	//updateFuelUI();

	player->runAction(MoveTo::create(2.0f, CP(0.5f, 0.1f)));
	scheduleOnce(schedule_selector(RaidLayer::switchToPlaying), 3.0f);
}

void RaidLayer::move(float angle, float radius)
{
	//abs(angle) < 90: right
	//abs(angle) > 90: left
	if (fabs(angle) > 80 && fabs(angle) < 100) _accel = 0;
	else if (fabs(angle) < 90) _accel = radius * PLAYER_SPEED;
	else if (fabs(angle) > 90) _accel = -radius * PLAYER_SPEED;
}

static float oldX = 0;
void RaidLayer::teleport(float x)
{
	if (state != GameStates::Playing)
	{
		oldX = x;
		return;
	}

	player->setPositionX(player->getPositionX() + x);
	oldX = x;
}

void RaidLayer::setBoost(float b)
{
	_boost = clamp(b, 0.7f, 5.0f);
	//_boost = -b + 0.5f;
	//_boost *= 2.75f;
}

void RaidLayer::shoot()
{
	if (state != GameStates::Playing) return;

	if (selectedWeapon == kBTN_LASER && rockets->getChildrenCount() == 0)
		rockets->addChild(Laser::create());
	else if (selectedWeapon == kBTN_ROCKET && rockets->getChildrenCount() == 0)
		rockets->addChild(Missile::create());
	else if (selectedWeapon == kBTN_GUN)
		rockets->addChild(Gun::create());
	else
		rockets->addChild(NormalLaser::create());
}

static int __raidframecounter = 0;
void RaidLayer::updateGame(float dt)
{
	if (VIEW_HEIGHT != oldViewHeight)
	{
		viewHeightChanged();
	}

	if (state == GameStates::Playing)
	{
		updatePlayerMovement(dt);
		updateRocketMovement(dt);
		removeInvisibleEntities();
		updatePhysics(dt);
		streamLevels();
	}
}

void RaidLayer::viewHeightChanged()
{
	oldViewHeight = VIEW_HEIGHT;
	updateSpeedUI();
	updateScoreTo1UpUI();
}

void RaidLayer::updatePlayerMovement(float dt)
{
	//do acceleration and movement on player (X)
	auto oldVel = player->vel;
	//player->vel += _accel * dt;
	player->vel = _accel * dt;
	if (_accel == 0) player->vel = 0;
	//player->vel -= player->vel * PLAYER_DAMP * dt;
	if (fabs(player->vel) < PLAYER_STOP_THRES) player->vel = 0;
	player->vel = clampf(player->vel, -PLAYER_MAX_SPEED, PLAYER_MAX_SPEED);

	auto oldScale = player->getScaleX();
	if (oldVel != 0 && player->vel == 0)
	{
		player->setSpriteFrame("raider");
		player->setScaleX(fabs(oldScale));
	}
	else if (player->vel < 0 && oldVel >= 0)
	{
		player->setSpriteFrame("raiderrt");
		player->setScaleX(-fabs(oldScale));
	}
	else if (player->vel > 0 && oldVel <= 0)
	{
		player->setSpriteFrame("raiderrt");
		player->setScaleX(fabs(oldScale));
	}
	player->moveX(player->vel);
	//((LabelTTF*) getChildByTag(1))->setString(nToString(levels->getPositionY()));

	//move player on (Y) 
	gameLayer->setPositionY(gameLayer->getPositionY() - _boost * PLAYER_SPEEDY * dt);
	updateSpeedUI();
	spendFuel(dt);
	/*
	auto flame = static_cast<ParticleSystemQuad*>(player->getChildByTag(kFLAME_PARTICLE));
	flame->setPosition(Point(player->getContentSize().width * 0.5f, 4));
	//flame->setStartColor(lerp(Color4F::RED, Color4F::GREEN, fuel));
	float speed = clampf(_boost - 0.5f, 0.0f, 3.5f);
	speed /= 3.5f;
	flame->setLife(speed);*/
	//flame->setStartSize(speed * 64.0f);
	//flame->setEndSize(speed * 64.0f);
}

void RaidLayer::updateRocketMovement(float dt)
{
	vector<Node*> rocketsToDelete;
	for (auto rocket : rockets->getChildren())
	{
		rocket->update(dt);
		if (rocket->getChildrenCount() == 0)
			rocketsToDelete.push_back(rocket);
	}
}

void RaidLayer::stopRocket()
{
	return;
}

void RaidLayer::updatePhysics(float dt)
{
	auto playerAABB = player->getBoundingBox();
	for (auto level : levels->getChildren())
	{
		vector<Entity*> entitiesToCollect;
		float offset = level->getPositionY() + levels->getPositionY() + gameLayer->getPositionY();
		for (auto obj : level->getChildren())
		{
			auto aabb = obj->getBoundingBox();
			aabb.origin.y += offset;
			Entity* e = dynamic_cast<Entity*>(obj);
			if (e)
			{	//we have an entity
				if (e->collect)
				{
					entitiesToCollect.push_back(e);
					continue;
				}
				else if (!e->dead)
				{
					vector<Node*> rocketsToDelete;
					for (auto rocket : rockets->getChildren())
					{
						for (auto projectile : rocket->getChildren())
						{
							auto rocketAABB = projectile->getBoundingBox();
							if (aabb.intersectsRect(rocketAABB))
							{
								if (!e->touched)
								{
									e->touched = true;
								}
								if (!e->ghost)
								{	//hit by rocket
									if (!e->immortal)
									{
										e->die();
										giveScore(e->scorePoints);
									}
									if (e->itemType == Items::Bridge)
									{
										auto bridgeAABB = e->getBoundingBox();
										createBridgeExplosion(Point(bridgeAABB.getMidX(), bridgeAABB.getMidY()));
										advanceLevel();
									}
									static_cast<Rocket*>(rocket)->hit(projectile, false);
									stopRocket();
									continue;
								}
							}
						}
						if (rocket->getChildrenCount() == 0) rocketsToDelete.push_back(rocket);
					}
					for (auto rocket : rocketsToDelete) rockets->removeChild(rocket);

					if (aabb.intersectsRect(playerAABB))
					{	//entity hit player
						if (e->itemType == Items::Fuel)
						{
							giveFuel(dt);
							continue;
						}
						else
						{
							if (!e->immortal) e->die();
							playerDie();
							return;
						}
					}
					if (RaidScene::Level > 0 && e->vel != 0 &&
						(e->getPositionY() + offset - player->getPositionY())
						< e->wakeUpDistance)
					{	//entity is awake, let's move it
						e->setPositionX(e->getPositionX() + e->vel * _entityBoost * dt);
						if (!e->noclip)
						{	//check for collision with walls and turn it
							bool turn = false;
							auto localaabb = obj->getBoundingBox();
							for (auto obst : level->getChildren())
							{
								if (!dynamic_cast<Entity*>(obst))
								{	//obstacle is a wall
									auto wallaabb = obst->getBoundingBox();
									if (wallaabb.intersectsRect(localaabb))
									{
										if (wallaabb.getMidX() < localaabb.getMinX())
										{	//wall is to the left of entity
											e->setPositionX(wallaabb.getMaxX() + 4);
										}
										else
										{	//wall is to the right of entity
											e->setPositionX(wallaabb.getMinX() - localaabb.size.width - 4);
										}
										turn = true;
									}
								}
								if (turn) break;
							}
							if (turn)
							{
								e->vel *= -1;
								e->setFlippedX(e->vel < 0);
							}
						}
					}
				}
			}
			else
			{	//object is terrain
				if (aabb.intersectsRect(playerAABB))
				{
					playerDie();
					return;
				}
				vector<Node*> rocketsToDelete;
				for (auto rocket : rockets->getChildren())
				{
					for (auto projectile : rocket->getChildren())
					{
						auto rocketAABB = projectile->getBoundingBox();
						if (aabb.intersectsRect(rocketAABB))
						{
							static_cast<Rocket*>(rocket)->hit(projectile, true);
							stopRocket();
						}
					}
					if (rocket->getChildrenCount() == 0) rocketsToDelete.push_back(rocket);
				}
				for (auto rocket : rocketsToDelete) rockets->removeChild(rocket);
			}
		}
		for (auto e : entitiesToCollect)
		{
			e->stopAllActions();
			e->unscheduleAllSelectors();
			level->removeChild(e);
		}
	}
}


void RaidLayer::playerDie()
{
    CocosDenshion::SimpleAudioEngine::getInstance()->stopAllEffects();
	if (state != GameStates::Playing) return;
	PlayEffect("explosion.wav", RANDF * 0.3f + 0.7f);
	rockets->removeAllChildren();
	lives--;
	state = GameStates::Dying;
	Layer* livesLayer = static_cast<Layer*>(getChildByTag(kLYR_LIVES));
	if (livesLayer->getChildrenCount() > 0)
		livesLayer->removeChild(
		*livesLayer->getChildren().rbegin());
	player->die();
	createPlayerExplosion();
	scheduleOnce(schedule_selector(RaidLayer::removePlayerExplosion), 2.0f);
	scheduleOnce(schedule_selector(RaidLayer::switchToTimer), 4.0f);
	//switchToLevelTxt();
}

void RaidLayer::removePlayerExplosion(float)
{
	static_cast<ParticleSystemQuad*>(canvas->getChildByTag(kPLAYER_EXPLOSION))->stopSystem();
}

void RaidLayer::advanceLevel()
{
	RaidScene::Level++;
	flashText(LS("Level") + " " + nToString(RaidScene::Level + 1));
    if (RaidScene::Level == 2)
        GameSettings::ReportAchievement(ACHIEVEMENT_RAID_ROOKIE);
    else if (RaidScene::Level == 4)
        GameSettings::ReportAchievement(ACHIEVEMENT_RAID_CAPTAIN);
    else if (RaidScene::Level == 8)
        GameSettings::ReportAchievement(ACHIEVEMENT_RAID_COLONEL);
    else if (RaidScene::Level == 16)
        GameSettings::ReportAchievement(ACHIEVEMENT_RAID_GENERAL);
    
	_entityBoost = SPEED_BOOST_EASY;
	if (RaidScene::Level >= 4) _entityBoost = SPEED_BOOST_MEDIUM;
	if (RaidScene::Level >= 8) _entityBoost = SPEED_BOOST_HARD;
	if (RaidScene::Level >= 12) _entityBoost = SPEED_BOOST_BRUTAL;
	switchToLevelTxt();
	Label* txtLevel = static_cast<Label*>(getChildByTag(kLYR_LEVEL)->getChildByTag(kTXT_LEVEL));
	txtLevel->setString(nToString(RaidScene::Level + 1));
	txtLevel->runAction(Sequence::createWithTwoActions(
		ScaleTo::create(0.1f, VALUESCALE * 1.2f), ScaleTo::create(0.1f, VALUESCALE)));
}

void RaidLayer::giveFuel(float dt)
{
	fuel += FUEL_GAIN * dt;
	if (fuel > 1) fuel = 1;

	_refueling = true;
	updateFuelUI();
}

void RaidLayer::spendFuel(float dt)
{
	fuel -= FUEL_USE * dt;
	if (fuel < 0)
	{
		fuel = 0;
		playerDie();
	}
	else
	{
		updateFuelUI();
	}
}

void RaidLayer::giveLife()
{
	scoreInLife %= SCORE_TO_LIFE;
	lives++;
    GameSettings::ReportAchievement(ACHIEVEMENT_SCORE_RAID);
	updateLivesUI();
}

void RaidLayer::giveScore(int _score)
{
	_score /= 10;
	score += _score;
	scoreInLife += _score;
	if (scoreInLife > SCORE_TO_LIFE) giveLife();
	Label* txtScore = static_cast<Label*>(getChildByTag(kLYR_SCORE)->getChildByTag(kTXT_SCORE));
	txtScore->setString(nToString(score));
	txtScore->runAction(Sequence::createWithTwoActions(
		ScaleTo::create(0.1f, VALUESCALE * 1.2f), ScaleTo::create(0.1f, VALUESCALE)));
	switchToScoreTxt();
	unschedule(schedule_selector(RaidLayer::switchToLevelTxt));
	scheduleOnce(schedule_selector(RaidLayer::switchToLevelTxt), 5.0);

	updateScoreTo1UpUI();
}

void RaidLayer::updateLivesUI()
{
	Layer* livesLayer = static_cast<Layer*>(getChildByTag(kLYR_LIVES));
	livesLayer->removeAllChildren();
	for (size_t i = 0; i < lives - 1 && i < 4; i++)
	{
		auto life = MAKESPRITE("raider");
		life->setScale(0.25f);
		livesLayer->addChild(life);
		life->setAnchorPoint(Point(0,1.2f));
		life->setPositionX(i * 0.3f * life->getContentSize().width);
        life->setPositionY(8);
		life->setColor(Color3B::BLACK);
		/*if (i == lives - 2)
		{
		life->setOpacity(0);
		life->runAction(Sequence::createWithTwoActions(
		DelayTime::create(0.2f + 0.1f * i), FadeIn::create(0.5f)));
		}*/
	}
}

static unsigned int soundEffectId = 666;
static bool vigFlashing = false;

void RaidLayer::updateFuelUI()
{
	/*
	auto dashboard = getChildByTag(kFUEL_DASHBOARD);
	auto fuelguage = dashboard->getChildByTag(kFUEL_GAUGE);
	fuelguage->setPositionX(fuel * dashboard->getContentSize().width);
	fuelguage->setAnchorPoint(Point(fuel, 1));
	fuelguage->setColor(tint);*/
	
	auto dashboardSize = getChildByTag(kFUEL_DASHBOARD)->getContentSize();
	auto fuelGuage = getChildByTag(kLYR_SPEEDBAR)->getChildByTag(kLYR_SPEEDGUAGE);
	auto color = lerp(Color3B(175,0,0), Color3B(0,150,0), fuel);
	fuelGuage->setColor(color);
	fuelGuage->setScaleY(lerp(fuel, fuelGuage->getScaleY(), 0.95f));

	auto fuelLabel = getChildByTag(kLBL_SPEED);
	fuelLabel->setPositionY(fuelGuage->getBoundingBox().getMaxY());

	if (fuel < 0.2f && !vigFlashing)
	{
		//soundEffectId = CocosDenshion::SimpleAudioEngine::getInstance()->
		//	playEffect("siren.wav", true, 2.0f, 0, 0.5f);
		getChildByTag(kLYR_CANVAS)->getChildByTag(kIMG_REDVIG)->runAction(
			RepeatForever::create(Sequence::createWithTwoActions(
			FadeIn::create(0.5f), FadeOut::create(0.5f))));
		vigFlashing = true;
		
	}
	else if (fuel > 0.3f && vigFlashing)
	{
		//CocosDenshion::SimpleAudioEngine::getInstance()->
		//	stopEffect(soundEffectId);
		getChildByTag(kLYR_CANVAS)->getChildByTag(kIMG_REDVIG)->stopAllActions();
		getChildByTag(kLYR_CANVAS)->getChildByTag(kIMG_REDVIG)->runAction(FadeOut::create(0.5f));
		vigFlashing = false;
        soundEffectId = 666;
	}
}

void RaidLayer::flashText(string text)
{
	winStatsLayer->setString(text);
	winStatsLayer->setPositionY(PY(-0.5f));
	winStatsLayer->runAction(Sequence::create(
		MoveTo::create(0.5f, CP(0.5f, 0.48f)),
		MoveTo::create(1.0f, CP(0.5f, 0.52f)),
		MoveTo::create(0.5f, CP(0.5f, 1.5f)),
		NULL));
	winStatsLayer->runAction(Sequence::create(
		FadeIn::create(0.4f),
		DelayTime::create(1.1f),
		FadeOut::create(0.3f),
		NULL));
}

static float softSpeed = 0;
static int lastSpeed = 0;
void RaidLayer::updateSpeedUI()
{
	float speed = clampf(_boost - 0.5f, 0.0f, 3.5f);
	speed /= 3.5f;

	softSpeed = lerp(speed, softSpeed, 0.99f);
	auto tint = lerp(Color3B(0, 0, 100), Color3B(255, 0, 0), softSpeed);
	if (lastSpeed != (int) (softSpeed * 100.0f))
	{
		lastSpeed = (int) (softSpeed * 100.0f);
		Label* l = static_cast<Label*>(getChildByTag(kTXT_SPEED));
		l->setString(nToString(lastSpeed) + "%");
		l->setColor(tint);
	}
}

void RaidLayer::updateScoreTo1UpUI()
{
	auto dashboardSize = getChildByTag(kFUEL_DASHBOARD)->getContentSize();
	auto lifeGuage = getChildByTag(kLYR_LIFEBAR)->getChildByTag(kLYR_LIFEGUAGE);
	float s = (float) scoreInLife / (float) SCORE_TO_LIFE;
	log("s: " + nToString(s) + "   scoreInLife: " + nToString(scoreInLife));
	lifeGuage->setColor(lerp(Color3B(0, 50, 100), Color3B(0, 100, 255), s));
	lifeGuage->stopAllActions();
	lifeGuage->runAction(ScaleTo::create(0.5f, lifeGuage->getScaleX(), s));

	auto lifeLabel = getChildByTag(kLBL_LIFE);
	lifeLabel->runAction(
		MoveTo::create(0.5f,
		Point(
		lifeLabel->getPositionX(),
		getChildByTag(kLYR_LIFEBAR)->getPositionY() +
		(PY(1.0f) - dashboardSize.height) * s
		)));
}

void RaidLayer::switchToPlaying(float)
{
	state = GameStates::Playing;
}

void RaidLayer::switchToTimer(float)
{
	if (lives <= 0) gameOver();
	else reloadGame();
}

void RaidLayer::switchToLevelTxt(float)
{
	return; //disabled
	if (!scoreActive) return;
	scoreActive = false;
	auto lyrScore = getChildByTag(kLYR_SCORE);
	auto lyrLevel = getChildByTag(kLYR_LEVEL);
	lyrScore->stopAllActions();
	lyrLevel->stopAllActions();
	lyrLevel->setPositionX(PX(0.5f));
	lyrLevel->runAction(MoveTo::create(0.5f, Point::ZERO));
	lyrScore->runAction(MoveTo::create(0.5f, Point(PX(0.5f), 0)));
	scheduleOnce(schedule_selector(RaidLayer::switchToScoreTxt), 10.0);
}

void RaidLayer::switchToScoreTxt(float)
{
	return; //disabled
	if (scoreActive) return;
	scoreActive = true;
	auto lyrScore = getChildByTag(kLYR_SCORE);
	auto lyrLevel = getChildByTag(kLYR_LEVEL);
	lyrScore->stopAllActions();
	lyrLevel->stopAllActions();
	lyrScore->setPositionX(PX(0.5f));
	lyrScore->runAction(MoveTo::create(0.5f, Point::ZERO));
	lyrLevel->runAction(MoveTo::create(0.5f, Point(PX(0.5f), 0)));
}

void RaidLayer::createBridgeExplosion(Point pos)
{
	PlayEffect("explosionbig.wav");
	ParticleSystemQuad* p = Entity::MakeExplosion();// ParticleSun::create();// ParticleSystemQuad::create("explosionbig/particle.plist");
	gameLayer->addChild(p, 4);
	p->setTag(kLVL_PARTICLE);
	p->setPositionType(tPositionType::RELATIVE);
	p->setPosition(pos);
	scheduleOnce(schedule_selector(RaidLayer::stopBridgeExplosion), 1.0f);
}

void RaidLayer::stopBridgeExplosion(float)
{
	static_cast<ParticleSystemQuad*>(gameLayer->getChildByTag(kLVL_PARTICLE))->stopSystem();
	scheduleOnce(schedule_selector(RaidLayer::removeBridgeExplosion), 1.0f);
}

void RaidLayer::removeBridgeExplosion(float)
{
	gameLayer->removeChildByTag(kLVL_PARTICLE);
}

void RaidLayer::createPlayerExplosion()
{
	ParticleSystemQuad* p = Entity::MakeExplosion();// ParticleSun::create();// ParticleSystemQuad::create("explosionbig/particle.plist");
	canvas->addChild(p);
	p->setTag(kPLAYER_EXPLOSION);
	p->setPositionType(tPositionType::RELATIVE);
	p->setPosition(player->getPosition());
    p->runAction(FadeOut::create(1.0f));
	player->runAction(FadeOut::create(0.3f));
	stopRocket();
}

void RaidLayer::playFuelSound(float dt)
{
	if (_refueling)
	{
		PlayEffect("fuel.wav", fuel);
		_refueling = false;
	}
}
void RaidLayer::gameOver()
{
	static_cast<RaidScene*>(getParent())->gameOver();
}
