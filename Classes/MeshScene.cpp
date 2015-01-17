#include "MeshScene.h"
#include "HelpScene.h"
#include "ExtraGameOverScene.h"
#include "BackgroundWidget.h"
#include "Ooze.h"

Layer* inventoryLayer;
static MenuItemSprite *btnSound;

const float BTNDELTA = 0.16f;
const int GAMEPARENTTAG = 5000;
const int OLDSTATSTAG = 5001;
const int NEWSTATSTAG = 5002;

const float GAMETIME = 60;

Scene* MeshScene::scene()
{
    GameSettings::MeshPlayed++;
    GameSettings::ReportAchievement(ACHIEVEMENT_UNLOCKED_MESH);
    GameSettings::TryGiveUnlockAll();
    GameSettings::SaveSettings();
    Ooze::InstantSend({{"Event", "GameStart"}, {"Game", "Mesh"}});
    GameSettings::OozeGameStats();
    Scene *scene = Scene::create();
    MeshScene *layer = MeshScene::create();
    scene->addChild(layer);
    return scene;
}

static const float buttonScale = 1.2f;
bool MeshScene::init()
{
	if (!Layer::init()) return false;

	auto bgcolor = LayerColor::create(GameInfos[Games::Mesh].color, DESIGN_X, DESIGN_Y);
	addChild(bgcolor);
    addChild(BackgroundWidget::createWithoutBackground());

	float menuY = 0.95f;
    
    auto hooker = TopAnchor::create();
    addChild(hooker, 1);
	pMenu = Menu::create();
	pMenu->setPosition(VCP(0.0f, menuY));
	hooker->addChild(pMenu, 1);
	
	float btnX = 0.00f;
	const float btnDelta = BTNDELTA;

	string sprite = GameSettings::MuteSounds ? "btnMute" : "btnSound";
	string psprite = sprite + "Dn";
	sprite += "Up";
	btnSound = MAKEBUTTON(sprite, psprite, btnSoundPressed);
	btnSound->setScale(buttonScale);
	pMenu->addChild(btnSound);
	btnSound->setPosition(VCP(0.95f, 0));
	btnSound->setAnchorPoint(Point(1, 1));
	btnX += btnDelta; 

	auto btnStop = MAKEBUTTON("btnExitUp", "btnExitDn", MeshScene::btnBackPressed);
	pMenu->addChild(btnStop);
	btnStop->setPosition(VCP(0.05f, 0));
	btnStop->setScale(buttonScale);
	btnStop->setAnchorPoint(Point(0, 1));
    
	over = false;

	createGame();

	auto keyboard = EventListenerKeyboard::create();
	keyboard->onKeyReleased = CC_CALLBACK_2(MeshScene::onKeyReleased, this);
	getEventDispatcher()->addEventListenerWithSceneGraphPriority(keyboard, this);

	auto touch = EventListenerTouchOneByOne::create();
	touch->onTouchBegan = CC_CALLBACK_2(MeshScene::onTouchBegan, this);
	touch->onTouchMoved = CC_CALLBACK_2(MeshScene::onTouchMoved, this);
	touch->onTouchEnded = CC_CALLBACK_2(MeshScene::onTouchEnded, this);
	touch->onTouchCancelled = CC_CALLBACK_2(MeshScene::onTouchEnded, this);
	getEventDispatcher()->addEventListenerWithSceneGraphPriority(touch, this);

    return true;
}

void MeshScene::btnNextLevel(Ref*)
{
	advanceLevel();
}

void MeshScene::btnBackPressed(Ref*)
{
	loseGame(true);
}


void MeshScene::btnSoundPressed(Ref*)
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

void MeshScene::btnHelpPressed(Ref*)
{
	Director::getInstance()->pushScene(TransitionFade::create(0.2f, HelpScene::scene()));
}

void MeshScene::createStats()
{
	float height = PY(0.2f);
	float lblFontSize = 32.0f;
	float valFontSize = 48.0f;
	float lblY = height - lblFontSize * 0.5f;
	float valY = lblY - valFontSize;
	Color3B statForeColor = Color3B::WHITE;

	auto bottomLayer = LayerColor::create(Color4B(0,0,0,0), PX(1.0f), height);
	addChild(bottomLayer);
	bottomLayer->setPosition(Point::ZERO);
	
	auto lblTime = Label::createWithTTF(LS("Time"), NORMAL_TTF, lblFontSize);
	lblTime->setHorizontalAlignment(TextHAlignment::CENTER);
	lblTime->setAnchorPoint(Point(0.5f, 1.0f));
	lblTime->setPosition(Point(PX(0.25f), lblY));
	lblTime->setColor(statForeColor);
	lblTime->setTag(10);
	bottomLayer->addChild(lblTime);

	auto valTime = Label::createWithBMFont(NORMAL_FNT, nToString(time) + LS("SecondSuffix"));
	valTime->setHorizontalAlignment(TextHAlignment::CENTER);
	valTime->setAnchorPoint(Point(0.5f, 1.0f));
	valTime->setPosition(Point(PX(0.25f), valY));
	valTime->setColor(statForeColor);
	valTime->setTag(11);
	bottomLayer->addChild(valTime);

	auto lblWins = Label::createWithTTF(LS("Points"), NORMAL_TTF, lblFontSize);
	lblWins->setHorizontalAlignment(TextHAlignment::CENTER);
	lblWins->setAnchorPoint(Point(0.5f, 1.0f));
	lblWins->setPosition(Point(PX(0.75f), lblY));
	lblWins->setColor(statForeColor);
	lblWins->setTag(10);
	bottomLayer->addChild(lblWins);

	auto valWins = Label::createWithBMFont(NORMAL_FNT, nToString(wins));
	valWins->setHorizontalAlignment(TextHAlignment::CENTER);
	valWins->setAnchorPoint(Point(0.5f, 1.0f));
	valWins->setPosition(Point(PX(0.75f), valY));
	valWins->setColor(statForeColor);
	valWins->setTag(11);
	bottomLayer->addChild(valWins);

	timerLayer = valTime;
	winsLayer = valWins;

	winStatsLayer = Label::createWithTTF(" ", NORMAL_TTF, valFontSize);
	winStatsLayer->setHorizontalAlignment(TextHAlignment::CENTER);
	winStatsLayer->setAnchorPoint(Point(0.5f, 1.0f));
	winStatsLayer->setPosition(Point(PX(0.5f), PY(-0.5f)));
	winStatsLayer->setColor(statForeColor);
	winStatsLayer->setTag(12);
	bottomLayer->addChild(winStatsLayer);
    
    bottomLayer->setPositionY(-valWins->boundingBox().size.height);
}

void MeshScene::onKeyReleased(EventKeyboard::KeyCode keyCode, Event* event)
{
	if (keyCode == EventKeyboard::KeyCode::KEY_ESCAPE)
		btnBackPressed(this);
	if (keyCode == EventKeyboard::KeyCode::KEY_N)
		btnNextLevel(this);
	if (keyCode == EventKeyboard::KeyCode::KEY_Z)
		time = 3;
}

bool MeshScene::onTouchBegan(Touch* touch, Event*)
{
	if (over) return false;

	lineBeginStuck = lineEndStuck = nullptr;
	swipeNextLevel = false;
	if (currentLine == nullptr)
	{
		currentLine = LineNode::create(touch->getLocation(), touch->getLocation(), Color4B::GRAY, 4.0f);
		game->addChild(currentLine, -1);
	}
	else
	{
		currentLine->setStart(touch->getLocation());
		currentLine->setEnd(touch->getLocation());
	}

	for (auto node : currentGraph->getChildren())
	{
		auto nodePos = currentGraph->getPosition() + node->getPosition();
		auto nodeRadius = static_cast<VertexNode*>(node)->circle->getContentSize().width * 1.0f;
		if ((touch->getLocation() - nodePos).getLength() < nodeRadius)
		{
			VertexNode* v = static_cast<VertexNode*>(node);
			if (allowStick(v))
			{
				lineBeginStuck = v;
				currentLine->setColor(Color4B::GRAY);
				currentLine->setStart(nodePos);
				break;
			}
		}
	}

	if (lineBeginStuck == nullptr)
	{
		//Two cases now: either we do line deletion, or we do swipe next level
		if (touch->getLocation().x > PX(0.8f)) swipeNextLevel = true;

#ifdef WIN32
		currentLine->setColor(Color4B::RED);
#else
		currentLine->setColor(Color4B(255,255,255,0));
#endif
	}

	return true;
}

void MeshScene::onTouchMoved(Touch* touch, Event*)
{
	currentLine->setEnd(touch->getLocation());

	if (swipeNextLevel)
	{
		float delta = touch->getLocation().x - touch->getStartLocation().x;
		//if (delta > -PX(0.1f))
			game->setPositionX(delta);
		//else
			//game->setSkewX(fabs(delta) / PX(1.0f));
	}
	else if (lineBeginStuck) onTouchMoved_connect(touch);
}

void MeshScene::onTouchMoved_connect(Touch* touch)
{
	auto oldStuck = lineEndStuck;
	lineEndStuck = nullptr;

	for (auto node : currentGraph->getChildren())
	{
		auto nodePos = currentGraph->getPosition() + node->getPosition();
		auto nodeRadius = static_cast<VertexNode*>(node)->circle->getContentSize().width * 1.0f;
		if ((touch->getLocation() - nodePos).getLength() < nodeRadius)
		{
			VertexNode* v = static_cast<VertexNode*>(node);
			if (oldStuck == v || (lineBeginStuck != v && allowStick(v)))
			{
				lineEndStuck = v;
				currentLine->setEnd(nodePos);
				break;
			}
		}
	}
}

void MeshScene::onTouchEnded(Touch* touch, Event*)
{
	if (lineBeginStuck && lineEndStuck && lineBeginStuck != lineEndStuck)
	{
		applyPair();
		return;
	}
	else if (swipeNextLevel)
	{
		if (touch->getLocation().x - touch->getStartLocation().x < -PX(0.1f))
		{
			for (auto child : game->getChildren())
			{
				if (!dynamic_cast<LineNode*>(child))
					child->setPositionX(child->getPositionX() + game->getPositionX());
				else
				{
					LineNode* ln = static_cast<LineNode*>(child);
					ln->setStart(ln->getStart() + game->getPosition());
					ln->setEnd(ln->getEnd() + game->getPosition());
				}
			}
			game->setPositionX(0);

			advanceLevel();
		}
		game->runAction(MoveTo::create(0.1f, Point::ZERO));
	}
	else if (lineBeginStuck == nullptr)
	{
		vector<Node*> removeQueue;
		for (auto child : game->getChildren())
		{
			LineNode* line = dynamic_cast<LineNode*>(child);
			if (line && line != currentLine && line->value != 0)
			{
				auto intersection = linesIntersect(
					line->getStart(),
					line->getEnd(),
					currentLine->getStart(),
					currentLine->getEnd());
				if (intersection)
				{
					line->tag1->setValue(line->tag1->getValue() + line->value);
					line->tag2->setValue(line->tag2->getValue() + line->value);
					line->tag1->shine();
					line->tag2->shine();

					PlayEffect("clean.wav",1,2);
					removeQueue.push_back(child);
					line->value = 0;
				}
			}
		}

		for (auto itr = removeQueue.begin(); itr != removeQueue.end(); itr++)
			game->removeChild(*itr);
	}

	currentLine->setStart(CP(-1.0f, -1.0f));
	currentLine->setEnd(CP(-1.0f, -1.1f));
}

static float gameY;
void MeshScene::createGame()
{
	over = false;
	brokeHighScore = false;

	game = Layer::create();
	game->setTag(GAME_TAG);
	addChild(game);
	swipeNextLevel = false;
	
	time = GAMETIME;
	level = 1;
	wins = 0;
	nodeCount = 2;
	defectChance = 0.0;

	currentLine = nullptr;
	prevGraph = nullptr;
	nextGraph = nullptr;

	bool defective = false;

    gameY = PY(0.5f);
	auto nodes = Graph::GenerateNodes(nodeCount, defective);
	currentGraph = Graph::GenerateUI(nodes, PX(0.25f));
	currentGraph->setPosition(PX(1.5f), gameY);
	game->addChild(currentGraph);

	currentGraph->runAction(MoveTo::create(0.5f, Point(PX(0.5f), gameY)));
	currentGraph->setTag(CURRENTGRAPH_TAG);

	float i = 0;
	for (auto node : currentGraph->getChildren())
	{
		i += 0.5f;
		Color3B color = hsl2Rgb(RANDF * 0.3f, 0.8f, 0.7f);
		static_cast<VertexNode*>(node)->tint(1.0f + i, color);
	}

	auto nextNodes = Graph::GenerateNodes(nodeCount + 1, defective);
	nextGraph = Graph::GenerateUI(nextNodes, PX(0.25f), 0, PY(0.05f));
	nextGraph->setPosition(CP(1.5f, 0.5f));
	game->addChild(nextGraph);
	nextGraph->setTag(NEXTGRAPH_TAG);

	createStats();
	schedule(schedule_selector(MeshScene::timerTick), 1);
}

void MeshScene::advanceLevel(float dt)
{
	bool defective = RANDF < defectChance;

	if (prevGraph != nullptr) game->removeChild(prevGraph);

	prevGraph = currentGraph;
	prevGraph->setTag(PREVGRAPH_TAG);
	
	
	currentGraph = nextGraph;
	currentGraph->runAction(MoveTo::create(0.5f, Point(PX(0.5f), gameY)));
	currentGraph->setTag(CURRENTGRAPH_TAG);
	
	float i = 0;
	for (auto node : currentGraph->getChildren())
	{
		i += 0.5f;
		Color3B color = hsl2Rgb(RANDF * 0.3f, 0.9f, 0.6f);
		static_cast<VertexNode*>(node)->tint(1.0f + i, color);
	}

	level++;
	nodeCount = min(2 + wins / 2, 6);

	auto nextNodes = Graph::GenerateNodes(nodeCount, defective);
	nextGraph = Graph::GenerateUI(nextNodes, PX(0.25f));
	nextGraph->setPosition(PX(1.5f), gameY);
	game->addChild(nextGraph);
	nextGraph->setTag(NEXTGRAPH_TAG);

	for (auto line : lines)
	{
		if (line->value > 0)
		{
			LineNode* newLine = LineNode::create(
				line->getStart() - prevGraph->getPosition(),
				line->getEnd() - prevGraph->getPosition(), 
				line->getColor(), line->getThickness());
			prevGraph->addChild(newLine, -1);
			game->removeChild(line);
		}
	}
	prevGraph->runAction(MoveTo::create(0.5f, Point(PX(-1.5f), gameY)));
	lines.clear();

	lineBeginStuck = lineEndStuck = nullptr;
}

bool MeshScene::allowStick(VertexNode* v)
{
	if (0 == v->getValue()) return false;
	PlayEffect("blimp_low1.wav", RANDF * 0.5f + 0.5f, 1.0f);
	v->shine();
	return true;
}

void MeshScene::applyPair()
{
	if (!lineBeginStuck || !lineEndStuck) return;
	
	int valBegin = lineBeginStuck->getValue();
	int valEnd = lineEndStuck->getValue();
	if (valBegin <= 0 || valEnd <= 0) return;

	if (valEnd > valBegin)
	{
		currentLine->value = valBegin;
		lineBeginStuck->setValue(0);
		lineEndStuck->setValue(valEnd - valBegin);
	}
	else
	{
		currentLine->value = valEnd;
		lineEndStuck->setValue(0);
		lineBeginStuck->setValue(valBegin - valEnd);
	}

	lines.push_back(currentLine);
	currentLine->tag1 = lineBeginStuck;
	currentLine->tag2 = lineEndStuck;

	currentLine = nullptr;

	checkWin();
}

void MeshScene::checkWin()
{
	for (auto node : currentGraph->getChildren())
	{
		if (static_cast<VertexNode*>(node)->getValue()) return;
	}
	winGame();
}

void MeshScene::winGame()
{
	wins++;
    if (wins == 30)
        GameSettings::ReportAchievement(ACHIEVEMENT_SCORE_MESH);
	time += 5;
	advanceLevel();
	timerTick();

	winsLayer->setString(nToString(wins));
	winsLayer->runAction(Sequence::createWithTwoActions(
		ScaleTo::create(0.1f, 1.2f),
		ScaleTo::create(0.2f, 1.0f)
		));

	if (wins > GameSettings::MeshHighScore)
	{
		if (!brokeHighScore)
		{
			brokeHighScore = true;
			winsLayer->runAction(TintTo::create(1.0f, 100, 255, 70));
            if (GameSettings::MeshHighScore > 9)
				flashText(LS("MeshBreakHighScore"));
		}
	}
}

void MeshScene::timerTick(float)
{
	if (time == GAMETIME)
	{
		//firsttime
        if (level == 1)
            flashText(LS("MeshGetReady"));
	}

	time--;
	if (time < 0)
	{
		loseGame();
	}
	else
	{
		timerLayer->setString(nToString(time) + LS("SecondSuffix"));
		
		if (time < 10)
		{
			timerLayer->runAction(Sequence::createWithTwoActions(
				TintTo::create(0.1f, 255, 100, 100),
				TintTo::create(0.2f, 255, 255, 255)
				));
			timerLayer->runAction(Sequence::createWithTwoActions(
				ScaleTo::create(0.1f, 1.2f),
				ScaleTo::create(0.2f, 1.0f)
				));
		}
		else if (time < 20)
		{
			timerLayer->runAction(Sequence::createWithTwoActions(
				ScaleTo::create(0.1f, 1.1f),
				ScaleTo::create(0.2f, 1.0f)
				));
		}
	}
}

void MeshScene::loseGame(bool backPressed)
{
	if (over) return;
	over = true;
	GameSettings::MeshLastScore = wins;

	if (GameSettings::MeshLastScore > GameSettings::MeshHighScore)
		GameSettings::MeshHighScore = GameSettings::MeshLastScore;

	GameSettings::MeshTotalScore += GameSettings::MeshLastScore;
    
    if (!backPressed && wins == 0)
        GameSettings::ReportAchievement(ACHIEVEMENT_LOSE_MESH);
	GameSettings::SaveSettings();
	
	Director::getInstance()->replaceScene(TransitionFade::create(1.0f, 
		ExtraGameOverScene::createScene(Games::Mesh)));
}

void MeshScene::flashText(string text)
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