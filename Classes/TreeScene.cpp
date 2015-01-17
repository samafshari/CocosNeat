#include "TreeScene.h"
#include "helpers.h"
#include "ExtraGameOverScene.h"
#include "BackgroundWidget.h"
#include "Ooze.h"

static bool helloAchieved = false;

Scene* TreeScene::scene()
{
    GameSettings::TreePlayed++;
    GameSettings::ReportAchievement(ACHIEVEMENT_UNLOCKED_TREE);
    GameSettings::TryGiveUnlockAll();
    GameSettings::SaveSettings();
    Ooze::InstantSend({{"Event", "GameStart"}, {"Game", "Tree"}});
    GameSettings::OozeGameStats();
    Scene *scene = Scene::create();
    TreeScene *layer = TreeScene::create();
	layer->readyGame();
    scene->addChild(layer);
    return scene;
}

bool TreeScene::init()
{
	if (!Layer::init()) return false;
	state = TreeGameStates::Initializing;
	treeTime = 0;
	Size visibleSize = Director::getInstance()->getVisibleSize();

	LayerColor* bgcolor = LayerColor::create(Color4B::WHITE, visibleSize.width, visibleSize.height);
    bgcolor->runAction(TintTo::create(4.0f * 60.0f, 0, 0, 0));
	this->addChild(bgcolor);

    auto hooker = TopAnchor::create();
    addChild(hooker, 1);
	auto pMenu = Menu::create();
	pMenu->setPosition(Point::ZERO);
	hooker->addChild(pMenu);

#if (CC_TARGET_PLATFORM != CC_PLATFORM_ANDROID)
	auto btnBack = MAKEBUTTON("btnMenuUp", "btnMenuDn", TreeScene::btnExitPressed);
	btnBack->setPosition(VCP(0.0f, 1.0f));
    btnBack->setAnchorPoint(Point(-0.5f, 1.5f));
	pMenu->addChild(btnBack);
#endif

	auto keyboard = EventListenerKeyboard::create();
	keyboard->onKeyReleased = CC_CALLBACK_2(TreeScene::onKeyReleased, this);
	getEventDispatcher()->addEventListenerWithSceneGraphPriority(keyboard, this);

	auto listener = EventListenerAcceleration::create(CC_CALLBACK_2(TreeScene::onAcceleration, this));
	getEventDispatcher()->addEventListenerWithSceneGraphPriority(listener, this);
		
	treeLayer = Layer::create();
	addChild(treeLayer);
    
	resetGame();
	
	scheduleUpdate();

	return true;
}

void TreeScene::onAcceleration(Acceleration* accel, Event*)
{
	if (accel->x < -0.1f) move(Directions::Left);
	else if (accel->x > 0.1f) move(Directions::Right);
	else move(Directions::Stop);
}


void TreeScene::btnExitPressed(Ref*){
	menuCloseCallback(nullptr);
}

void TreeScene::menuCloseCallback(Ref*){
	die();
}

void TreeScene::resetGame()
{
	state = TreeGameStates::Initializing;
	score = 0;
	highScoreBeaten = false;
	treeLayer->removeAllChildren();
    treeVelocity = TREEVELOCITY;
	manDirection = Directions::Stop;
	man = MAKESPRITE("manUp");
	man->setPosition(1000, 2000);
	man->setScale(0.8f);
	addChild(man);
	
	for (int i = 0; i < TREECOUNT; i++)
	{
		Sprite* tree = MAKESPRITE(getTree());
		treeLayer->addChild(tree);
		tree->setTag(100 + i);
		tree->setPosition(ILLEGAL_POSITION);
	}
}

void TreeScene::startGame(float)
{
	state = TreeGameStates::Ready;

	lblScore = Label::createWithBMFont("font.fnt", "Score: " + nToString(score));
	lblScore->setScale(0.7f);
	lblScore->setPosition(CP(0.05f, 0.05f));
	lblScore->setAnchorPoint(Point(0.0f, 0.0f));
	lblScore->setColor(Color3B::BLACK);
	addChild(lblScore);

	lblHighScore = Label::createWithBMFont("font.fnt", "High Score: " + nToString(GameSettings::TreeHighScore));
	lblHighScore->setScale(0.7f);
	lblHighScore->setPosition(CP(0.95, 0.05f));
	lblHighScore->setAnchorPoint(Point(1.0f, 0.0f));
	lblHighScore->setColor(Color3B::BLACK);
	addChild(lblHighScore);

}

void TreeScene::readyGame(float)
{
	man->setPosition(CP(0.5f, -0.1f));
	man->runAction(MoveBy::create(1.0f, CP(0.0f, 0.25f)));
	scheduleOnce(schedule_selector(TreeScene::startGame), 1.0f);
}

void TreeScene::die()
{
	unscheduleAllSelectors();
	state = TreeGameStates::Done;
	GameSettings::TreeLastScore = score;
	if (score > GameSettings::TreeHighScore)
		GameSettings::TreeHighScore = score;
	GameSettings::TreeTotalScore += GameSettings::TreeLastScore;

	GameSettings::SaveSettings();
	Director::getInstance()->replaceScene(TransitionFade::create(1.0f, 
		ExtraGameOverScene::createScene(Games::Tree)));
}

void TreeScene::resetTree(Sprite* tree)
{
	tree->setSpriteFrame(getTree());
	tree->setPositionX(PX(RANDF));
	tree->setPositionY(PY(1.0f + RANDF * 2.0f));
	treeTime = 0;
}

void TreeScene::update(float dt)
{
	if (state == TreeGameStates::Done) return;
    if (state == TreeGameStates::Ready) treeVelocity += treeVelocity * 0.01f * dt;
	treeTime += dt;
	
	auto manPos = man->getPosition();
	auto manSize = man->getContentSize() * man->getScale(); //or getBoundingBox()?
	auto manBox = man->getBoundingBox();
	manBox.size.width *= 0.5f;
	manBox.size.height *= 0.8f;

	if (state == TreeGameStates::Ready)
	{
		if (manDirection == Directions::Left) manPos.x -= PX(MANVELOCITY * dt);
		else if (manDirection == Directions::Right) manPos.x += PX(MANVELOCITY * dt);

		if (manPos.x - manSize.width * 0.5f < 0) manPos.x = manSize.width * 0.5f;
		else if (manPos.x + manSize.width * 0.5 > DESIGN_X) manPos.x = DESIGN_X - manSize.width * 0.5f;
		man->setPositionX(manPos.x);
	}

	bool treeMade = false;
	for (int i=0; i < TREECOUNT; i++)
	{
		Sprite* tree = static_cast<Sprite*>(treeLayer->getChildByTag(100+i));
		auto treePos = tree->getPosition();
		if (treePos == ILLEGAL_POSITION)
		{
			if (RANDF < TREECHANCE && !treeMade && treeTime > TREETIME)
			{
				treeMade = true;
				resetTree(tree);
			}
			else continue;
		}

 		tree->setPositionY(tree->getPositionY() - PY(treeVelocity * dt));
		
		auto box = tree->getBoundingBox();
		if (box.getMaxY() < 0)
		{
			tree->setPosition(ILLEGAL_POSITION);
			giveScore(1);
			continue;
		}

		box.size.width *= 0.4f;
		box.size.height *= 0.7f;
		if (state == TreeGameStates::Ready && manBox.intersectsRect(box))
		{
			die();
			return;
		}
	}
}

void TreeScene::giveScore(int points)
{
    int oldScore = score;
	if (state != TreeGameStates::Ready) return;
	score += points;
	lblScore->setString("Score: " + nToString(score));

	if (score > GameSettings::TreeHighScore)
	{
		if (!highScoreBeaten)
		{
			highScoreBeaten = true;
			lblHighScore->runAction(RepeatForever::create(Sequence::createWithTwoActions(
				TintTo::create(0.25f, 255, 0, 0), TintTo::create(0.25f, 0, 0, 0))));
		}

		lblHighScore->setString("High Score: " + nToString(score));
        /*
#if IS_IOS
        if (!helloAchieved && score > 30)
        {
            helloAchieved = true;
            GKHWrapperCpp gkh;
            gkh.reportAchievement("TreeMassacreAchievementHello", 100, true);
        }
#endif*/
        if (oldScore < 500 && score >= 500)
            GameSettings::ReportAchievement(ACHIEVEMENT_SCORE_TREE);
	}
}
void TreeScene::move(Directions dir)
{
	if (state != TreeGameStates::Ready) return;
	if (manDirection != dir)
	{
		if (dir == Directions::Left) man->setSpriteFrame("manLeft");
		else if (dir == Directions::Right) man->setSpriteFrame("manRight");
		else if (dir == Directions::Stop) man->setSpriteFrame("manUp");
		//default: this shouldn't happen.
	}
	manDirection = dir;
}

void TreeScene::onKeyReleased(EventKeyboard::KeyCode keyCode, Event* event)
{
	if (keyCode == EventKeyboard::KeyCode::KEY_ESCAPE)
		btnExitPressed(nullptr);
}
