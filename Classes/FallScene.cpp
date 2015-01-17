#include "FallScene.h"
#include "ExtraGameOverScene.h"
#include "BackgroundWidget.h"
#include "GameSettings.h"
#include "Ooze.h"

Scene* FallScene::createScene()
{
    GameSettings::FallPlayed++;
    GameSettings::ReportAchievement(ACHIEVEMENT_UNLOCKED_FALL);
    GameSettings::TryGiveUnlockAll();
    GameSettings::SaveSettings();
    Ooze::InstantSend({{"Event", "GameStart"}, {"Game", "Fall"}});
    GameSettings::OozeGameStats();
	Scene* scene = Scene::createWithPhysics();
	//scene->getPhysicsWorld()->setDebugDrawMask(PhysicsWorld::DEBUGDRAW_ALL);
	scene->getPhysicsWorld()->setGravity(Vect(0, -2000));
	auto layer = FallScene::create();
	layer->world = scene->getPhysicsWorld();
	scene->addChild(layer);
	return scene;
}

bool FallScene::init()
{
	if (!Layer::init()) return false;
	Size visibleSize = Director::getInstance()->getVisibleSize();

	auto bg = LayerColor::create(Color4B(60, 0, 100, 255), visibleSize.width, visibleSize.height);
    bg->runAction(TintTo::create(2.0f * 60.0f, 0, 0, 0));
	addChild(bg);

	addChild(BackgroundWidget::createWithoutBackground());

	auto vig = Sprite::create("vig.png");
	auto vigSize = vig->getContentSize();
	vig->setScaleX(visibleSize.width / vigSize.width);
	vig->setScaleY(visibleSize.height / vigSize.height);
	vig->setPosition(CP(0.5f, 0.5f));
	addChild(vig);

	PhysicsMaterial material;
	material.restitution = 0.80f;

	ball = MAKESPRITE("ball");
	ball->setOpacity(0);
	ball->setPosition(CP(0.5f, 0.9f));
	ball->setScale(0.75f);
	ball->setPhysicsBody(PhysicsBody::createCircle(ball->getBoundingBox().size.width * 0.5f, material));
	ball->getPhysicsBody()->setMass(100000);
	ball->getPhysicsBody()->setVelocityLimit(600);
	ball->getPhysicsBody()->setGravityEnable(false);
	ball->setScale(0.01f);
	ball->runAction(ScaleTo::create(2.0f, 0.75f));
	ball->runAction(RotateBy::create(0.5f, 365));
	ball->runAction(FadeIn::create(2.0f));
	addChild(ball);

	rects = Node::create();
	addChild(rects);
    auto hooker = TopAnchor::create();
    addChild(hooker, 1);
	auto pMenu = Menu::create();
	pMenu->setPosition(Point::ZERO);
	hooker->addChild(pMenu);

#if (CC_TARGET_PLATFORM != CC_PLATFORM_ANDROID)
	auto btnBack = MAKEBUTTON("btnMenuUp", "btnMenuDn", die);
	pMenu->addChild(btnBack);
	btnBack->setPosition(VCP(0.0f, 1.0f));
    btnBack->setAnchorPoint(Point(-0.5f, 1.5f));
#endif

	auto keyboard = EventListenerKeyboard::create();
	keyboard->onKeyReleased = CC_CALLBACK_2(FallScene::onKeyReleased, this);
	getEventDispatcher()->addEventListenerWithSceneGraphPriority(keyboard, this);

	auto listener = EventListenerAcceleration::create(CC_CALLBACK_2(FallScene::onAcceleration, this));
	getEventDispatcher()->addEventListenerWithSceneGraphPriority(listener, this);
	
	dx = 0.0f;
	ballSpeed = 1700.0f;
	speed = 250.0f;
	genAlpha = 1.0f;
	genTime = 1.0f;
	time = 0;

	timeLabel = Label::createWithBMFont(NORMAL_FNT, "0" + LS("SecondSuffix"));
	timeLabel->setPosition(VCP(0.5f, 0.9f));
	timeLabel->setColor(Color3B::WHITE);
	timeLabel->setHorizontalAlignment(TextHAlignment::CENTER);
	addChild(timeLabel);
    
	schedule(schedule_selector(FallScene::updateTime), 1.0f);

	return true;
}

void FallScene::onAcceleration(Acceleration* accel, Event*)
{
	dx = accel->x;
}

void FallScene::onKeyReleased(EventKeyboard::KeyCode keyCode, Event* event)
{
	if (keyCode == EventKeyboard::KeyCode::KEY_ESCAPE)
		die();
}

void FallScene::updateTime(float)
{
	time++;
    if (time == 5 * 60)
        GameSettings::ReportAchievement(ACHIEVEMENT_SCORE_FALL);
    
	GameSettings::FallLastScore = time;
	timeLabel->setString(nToString(time) + "s");
	if (GameSettings::FallHighScore <= GameSettings::FallLastScore)
	{
		GameSettings::FallHighScore = GameSettings::FallLastScore;
		timeLabel->setColor(Color3B::GREEN);
		timeLabel->runAction(Sequence::createWithTwoActions(
			ScaleTo::create(0.2f, 1.2f), ScaleTo::create(0.1f, 1.0f)));
	}
	if (time == 1)
	{
		scheduleUpdate();
	}
	if (time == 2)
	{
		ball->getPhysicsBody()->setGravityEnable(true);
	}
}

void FallScene::update(float dt)
{
	//generate blocks
	genAlpha += dt;
	if (genAlpha > genTime)
	{
		PhysicsMaterial material;
		/*material.density = 10000;
		material.friction = 0;
		material.restitution = 1;*/


		genAlpha = 0;
		auto color = hsl2Rgb(RANDF, 0.8f, 0.8f);
		float gap = PX(0.2f);
		float height = PY(0.05f);
		float gapCenter = RANDF * (PX(1.0f) - gap) + gap * 0.5f;
		auto left = LayerColor::create(Color4B(color), gapCenter - gap * 0.5f, height);
		left->ignoreAnchorPointForPosition(false);
		left->setAnchorPoint(Point::ZERO);
		auto leftBody =  PhysicsBody::createBox(left->getBoundingBox().size, material);
			//PhysicsBody::createEdgeBox(left->getBoundingBox().size, material);
		leftBody->setGravityEnable(false);
		leftBody->setMass(PHYSICS_INFINITY);
		leftBody->setRotationEnable(false);
		//leftBody->setVelocityLimit(0);
		//leftBody->setResting(false);
		//leftBody->setDynamic(false);
		//leftBody->setVelocity(Vect(0, vel));
		left->setPhysicsBody(leftBody);
		
		left->setPositionX(left->getBoundingBox().size.width * 0.5f);
		left->setPositionY(-PY(0.1f));
		rects->addChild(left);

		auto right = LayerColor::create(Color4B(color), PX(1.0f) - (gapCenter + gap * 0.5f), height);
		right->ignoreAnchorPointForPosition(false);
		right->setAnchorPoint(Point::ZERO);
		auto rightBody = PhysicsBody::createBox(right->getBoundingBox().size, material);
			//PhysicsBody::createEdgeBox(right->getBoundingBox().size, material);
		rightBody->setGravityEnable(false);
		rightBody->setMass(PHYSICS_INFINITY);
		rightBody->setRotationEnable(false);
		//rightBody->setVelocity(Vect(0, vel));
		//rightBody->setVelocityLimit(0);
		//rightBody->setResting(false);
		//rightBody->setDynamic(false);
		right->setPhysicsBody(rightBody);
		right->setPositionX(right->getBoundingBox().size.width * 0.5f + gapCenter + gap * 0.5f);
		right->setPositionY(-PY(0.1f));
		
		rects->addChild(right);
	}

	//move player
	/*ball->setPositionX(kmLerp(
		ball->getPositionX() + ballSpeed * dt * dx,
		ball->getPositionX(),
		0.5f));
		*/
	ball->getPhysicsBody()->setVelocity(Vect(ballSpeed * dx,ball->getPhysicsBody()->getVelocity().y));
	ball->setRotation(ball->getRotation() + 45.0f * dt + dx * dt * 1500.0f);

	//drop player
	//ball->setPositionY(ball->getPositionY() - gravity * dt);

	//limit x
	auto ballRect = ball->getBoundingBox();
	if (ballRect.getMinX() < 0) ball->setPositionX(ballRect.size.width * 0.5f);
	if (ballRect.getMaxX() > PX(1.0f)) ball->setPositionX(PX(1.0f) - ballRect.size.width * 0.5f);
	ballRect = ball->getBoundingBox();

	//ball collision
	//for (auto rp : rects->getChildren())
	//{
	//	auto aabb = rp->getBoundingBox();
	//	if (aabb.intersectsRect(ballRect))
	//	{
	//		const float lrThres = 0.1f;
	//		if (ballRect.getMinX() + ballRect.size.width * lrThres > aabb.getMaxX()) //right of left platform, in gap
	//			ball->setPositionX(aabb.getMaxX() + ballRect.size.width * 0.51f);
	//		else if (ballRect.getMaxX() - ballRect.size.width * lrThres < aabb.getMinX())
	//			ball->setPositionX(aabb.getMinX() - ballRect.size.width * 0.51f);
	//		else //keep ball on top
	//			ball->setPositionY(aabb.getMaxY() + ballRect.size.height * 0.5f);

	//		break;
	//	}
	//}

	//move & remove rects
	vector<Node*> rectsToRemove;
	for (auto rp : rects->getChildren())
	{
		//rp->setPositionY(rp->getPositionY() + speed * dt);
		rp->getPhysicsBody()->setVelocity(Vect(0, speed));
		if (rp->getBoundingBox().getMinY() > PY(1.1f)) rectsToRemove.push_back(rp);
	}
	for (auto rp : rectsToRemove) rects->removeChild(rp);

    speed += dt;
	//check gameover
	ballRect = ball->getBoundingBox();
	if (ballRect.getMinY() > PY(1.0f))// || ballRect.getMaxY() < 0)
		die();
    if (ball->getPositionY() < 0) ball->setPositionY(0);
    
}

void FallScene::die(Ref*)
{
	unscheduleAllSelectors();
	GameSettings::SaveSettings();
	GameSettings::FallTotalScore += GameSettings::FallLastScore;
	Director::getInstance()->replaceScene(TransitionFade::create(1.0f, 
		ExtraGameOverScene::createScene(Games::Fall)));
}