#include "ContrastScene.h"
#include "ExtraGameOverScene.h"
#include "Ooze.h"
#include "BackgroundWidget.h"
#include "Widgets.h"
#include "ShaderLayer.h"

namespace Contrast
{
	int ContrastScene::LastScore = 0;
	int ContrastScene::HighScore = 0;
	int ContrastScene::TotalScore = 0;

	Scene* ContrastScene::createScene()
	{
		Ooze::InstantSend({ { "Event", "GameStart" }, { "Game", "Contrast" } });
		GameSettings::OozeGameStats();
		Scene* scene = Scene::create();
		auto layer = ContrastScene::create();
		scene->addChild(layer);
		return scene;
	}

	bool ContrastScene::init()
	{
		if (!Layer::init()) return false;
		log("MiniMainMenu started.");

		rotationDirection = 1.0f;
        LastScore = 0;
		lives = MAX_LIVES;

		auto bgcolor = LayerColor::create(GameInfos[Games::Contrast].color, DESIGN_X, DESIGN_Y);
		addChild(bgcolor);

		livesLayer = Node::create();
		livesLayer->setPosition(CP(0.5f, 0.5f));
		addChild(livesLayer);
		updateLivesUI();

		auto pMenu = Menu::create();
		pMenu->setPosition(Point::ZERO);
		addChild(pMenu);

        auto btnBack = MakeButtonTextBelow("Platformer_pauseup", "Platformer_pausedn", LS("Pause"),
            24.0f, CC_CALLBACK_1(ContrastScene::btnPausePressed, this),
            NORMAL_TTF, COLOR3B(20,20,20));
        
        //MAKEBUTTON("btnExitUp", "btnExitDn", showGameOver);
        pMenu->addChild(btnBack);
		btnBack->setPosition(VCP(0.05f, 0.95f));
		btnBack->setAnchorPoint(Point(0.0f, 1.0f));

		auto help = Label::createWithTTF(LS("CONTRAST_HELP_ONELINE"), NORMAL_TTF, 24.0);
		addChild(help);
		help->setColor(Color3B(20,20,20));
		help->setHorizontalAlignment(TextHAlignment::CENTER);
		help->setPositionX(VPX(0.5f));
		help->setPositionY(VPY(0.92f));
		
        auto btnNext = MakeButtonTextBelow("arrow_next_up", "arrow_next_dn", LS("Skip"),
             24.0f, CC_CALLBACK_1(ContrastScene::skip, this),
             NORMAL_TTF, COLOR3B(20,20,20));
        
        //MAKEBUTTON("arrow_next_up", "arrow_next_dn", skip);
		pMenu->addChild(btnNext);
		btnNext->setPosition(VCP(0.95f, 0.95f));
		btnNext->setAnchorPoint(Point(1.0f, 1.0f));

        auto touchListener = EventListenerTouchOneByOne::create();
        touchListener->onTouchBegan = CC_CALLBACK_2(ContrastScene::onTouchBegan, this);
        getEventDispatcher()->addEventListenerWithSceneGraphPriority(touchListener, this);

		auto keyboard = EventListenerKeyboard::create();
		keyboard->onKeyReleased = CC_CALLBACK_2(ContrastScene::onKeyReleased, this);
		getEventDispatcher()->addEventListenerWithSceneGraphPriority(keyboard, this);

		maxsize = INIT_SIZE;
		distance = INIT_DISTANCE;
		time = MAX_TIME;
		wins = 0;
		bonustime = 0;
		bonusmultiplier = 1;

		previous = nullptr;
		current = nullptr;
#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)
		shader = Layer::create();
		shader->setScale(1.25f);
#else
		shader = ShaderLayer::create("generic.psh");
#endif
		addChild(shader);

		createStats();
		resumeSchedules();

		block_sprite = "contrast_block";
		auto block_template = MAKESPRITE(block_sprite);
		blockSize = block_template->getContentSize().width;
        
        colors =
        {
            Color3B::RED,
            Color3B(0,172,0),
            Color3B::BLUE,
            Color3B::ORANGE,
            Color3B::BLACK,
            Color3B::MAGENTA
        };
        labels =
        {
            "Red",
            "Green",
            "Blue",
            "Orange",
            "Black",
            "Purple"
        };
        
		layouts = vector < function<vector<Point>()> > {
			CC_CALLBACK_0(ContrastScene::createBar, this),
            CC_CALLBACK_0(ContrastScene::createPlus, this),
            CC_CALLBACK_0(ContrastScene::createSquare, this),
			
		};

		gameModes = vector < function<Vec2(vector<Point>)> > {
			CC_CALLBACK_1(ContrastScene::createColor, this),
			CC_CALLBACK_1(ContrastScene::createColorWordNormal, this),
			CC_CALLBACK_1(ContrastScene::createColorWordOpposite, this),
			CC_CALLBACK_1(ContrastScene::createColorSpellNormal, this),
			CC_CALLBACK_1(ContrastScene::createColorSpellOpposite, this)
		};

		nextBoard();

		return true;
	}

	void ContrastScene::onAcceleration(Acceleration* accel, Event*)
	{
	}

	void ContrastScene::onKeyReleased(EventKeyboard::KeyCode keyCode, Event* event)
	{
		if (keyCode == EventKeyboard::KeyCode::KEY_ESCAPE)
			//showGameOver();
			btnPausePressed();
	}

	void ContrastScene::update(float dt)
	{
	}

	void ContrastScene::showGameOver(Ref*)
	{
		unscheduleAllSelectors();

		if (LastScore > HighScore) HighScore = LastScore;
		TotalScore += LastScore;

		SaveSettings();
		Director::getInstance()->replaceScene(TransitionFade::create(1.0f, 
			ExtraGameOverScene::createScene(Games::Contrast)));
	}

	void ContrastScene::LoadSettings()
	{
		auto d = UserDefault::getInstance();
		LastScore = d->getIntegerForKey("ContrastLastScore", LastScore);
		HighScore = d->getIntegerForKey("ContrastHighScore", HighScore);
		TotalScore = d->getIntegerForKey("ContrastTotalScore", TotalScore);
	}

	void ContrastScene::SaveSettings()
	{
		auto d = UserDefault::getInstance();
		d->setIntegerForKey("ContrastLastScore", LastScore);
		d->setIntegerForKey("ContrastHighScore", HighScore);
		d->setIntegerForKey("ContrastTotalScore", TotalScore);
		GameSettings::SaveSettings();
	}

	void ContrastScene::skip(Ref*)
	{
        bonustime = 0.01f;
		nextBoard();
	}

	void ContrastScene::nextBoard()
	{
		previous = current;
        
		if (previous != nullptr)
			shader->removeChild(previous);

		current = Menu::create();
		current->setPosition(CP(0.5f, 0.45f));
		current->setAnchorPoint(Point::ZERO);

        //int gameModeSize = static_cast<int>(gameModes.size());
		
        int layoutSize = 1;
        int gameModeSize = 1;
        
        if (LastScore > 5)
        {
            layoutSize = static_cast<int>(layouts.size());
        }
        if (LastScore > 10)
        {
            gameModeSize = 2;
        }
        if (LastScore > 15)
        {
            gameModeSize = 3;
        }
        
		Vec2 p = gameModes[rand() % gameModeSize](
                    layouts[rand() % layoutSize]());

		current->setPosition(
			(DESIGN_X) * 0.5f,
            PY(0.05f) + (DESIGN_Y) * 0.4f);
	
		//enableRotation(p);

		shader->addChild(current);
	}

	void ContrastScene::win(Ref* r)
	{
		if (bonustime == 0)
		{
			bonusLayer->getChildByName("circHalf")->runAction(FadeIn::create(0.3f));
			bonusLayer->getChildByName("circHalf")->runAction(ScaleTo::create(0.3f, 0.6f));
			bonusLayer->getChildByName("valBonusTimer")->runAction(FadeIn::create(0.5f));
			bonusLayer->getChildByName("valMul")->runAction(MoveBy::create(0.2f, Point(0, -blockSize*0.1f)));
		}
		LastScore+=bonusmultiplier++;
		bonusmultiplier = min(bonusmultiplier, MAX_MULTIPLIER);

		bonustime = MAX_BONUS_TIME;
		wins++;
		
		static_cast<Label*>(bonusLayer->getChildByName("valMul"))->setString(
			"x" + nToString(bonusmultiplier));
		bonusLayer->getChildByName("circBonus")->runAction(Sequence::createWithTwoActions(
			ScaleTo::create(0.1f, 0.6f), ScaleTo::create(0.3f, 0.5f)));

		time += 1.0f;
		winsLayer->setString(nToString(LastScore));
		winsLayer->runAction(Sequence::createWithTwoActions(
			ScaleTo::create(0.1f, 1.0f),
			ScaleTo::create(0.2f, 0.75f)
			));

		if (LastScore % 5 == 0)
			maxsize++;

		distance *= 0.9f;

		maxsize = min(maxsize, MAX_SIZE);
		distance = max(distance, MIN_DISTANCE);

		nextBoard();

		Node* b = dynamic_cast<Node*>(r);
		if (b)
		{
			b->runAction(Sequence::createWithTwoActions(
				ScaleTo::create(0.1f, 0.8f), ScaleTo::create(0.3f, 1.0f)));
		}

		float freq = 0.5f - 0.5f * min(1.0f,((float)bonusmultiplier / 7.0f));
		PlayEffect("contrast_win.wav", freq);
	}

	void ContrastScene::secret(Ref*)
	{

	}

	void ContrastScene::dummy(Ref* r)
	{
		Node* b = dynamic_cast<Node*>(r);
		if (b)
		{
			b->runAction(Sequence::createWithTwoActions(
				ScaleTo::create(0.2f, 0.8f), ScaleTo::create(0.3f, 1.0f)));
		}

		log("Wrong.");
		PlayEffect("wrong.wav");
		lives--;
		updateLivesUI();
		if (!lives) showGameOver();
	}

	void ContrastScene::createStats()
	{
		float height = PY(0.2f);
		float lblFontSize = 24.0f;
		float valFontSize = 24.0f;
		float lblY = height - lblFontSize * 0.5f;
		float valY = lblY - valFontSize;

		Color3B statForeColor = Color3B::BLACK;
		Color3B circColor = Color3B(220, 220, 220);

		auto bottomLayer = LayerColor::create(Color4B(0, 0, 0, 0), PX(1.0f), height);
		addChild(bottomLayer);
		bottomLayer->setPosition(Point::ZERO);
		bottomLayer->setPositionY(-PY(0.05f));

		auto circTime = MAKESPRITE("circle_large");
		circTime->setColor(circColor);
		circTime->setScale(0.5f);
		bottomLayer->addChild(circTime);

		auto lblTime = Label::createWithTTF(LS("Time"), NORMAL_TTF, lblFontSize);
		lblTime->setHorizontalAlignment(TextHAlignment::CENTER);
		lblTime->setAnchorPoint(Point(0.5f, 0.95f));
		lblTime->setPosition(Point(PX(0.25f), lblY));
		lblTime->setColor(statForeColor);
		lblTime->setTag(10);
		bottomLayer->addChild(lblTime);

		auto valTime = Label::createWithBMFont(NORMAL_FNT, nToString(time) + LS("SecondSuffix"));
		valTime->setHorizontalAlignment(TextHAlignment::CENTER);
		valTime->setAnchorPoint(Point(0.5f, 1.05f));
		valTime->setPosition(Point(PX(0.25f), valY));
		valTime->setColor(statForeColor);
		valTime->setTag(11);
		valTime->setScale(0.75f);
		bottomLayer->addChild(valTime);

		circTime->setPositionX(valTime->getPositionX());
		circTime->setPositionY(0.5f * (valTime->getBoundingBox().getMaxY() + lblTime->getBoundingBox().getMinY()));

		auto circWins = MAKESPRITE("circle_large");
		circWins->setColor(circColor);
		circWins->setScale(0.5f);
		bottomLayer->addChild(circWins);

		auto lblWins = Label::createWithTTF(LS("Score"), NORMAL_TTF, lblFontSize);
		lblWins->setHorizontalAlignment(TextHAlignment::CENTER);
		lblWins->setAnchorPoint(Point(0.5f, 0.95f));
		lblWins->setPosition(Point(PX(0.75f), lblY));
		lblWins->setColor(statForeColor);
		lblWins->setTag(10);
		bottomLayer->addChild(lblWins);

		auto valWins = Label::createWithBMFont(NORMAL_FNT, nToString(LastScore));
		valWins->setHorizontalAlignment(TextHAlignment::CENTER);
		valWins->setAnchorPoint(Point(0.5f, 1.05f));
		valWins->setPosition(Point(PX(0.75f), valY));
		valWins->setColor(statForeColor);
		valWins->setTag(11);
		valWins->setScale(0.75f);
		bottomLayer->addChild(valWins);

		circWins->setPositionX(valWins->getPositionX());
		circWins->setPositionY(0.5f * (valWins->getBoundingBox().getMaxY() + lblWins->getBoundingBox().getMinY()));

		bonusLayer = Node::create();
		auto circBonusHalf = MAKESPRITE("circle_line");
		circBonusHalf->setScale(0.1f);
		circBonusHalf->setColor(Color3B::GREEN);
		circBonusHalf->setName("circHalf");
		circBonusHalf->setOpacity(0);
		bonusLayer->addChild(circBonusHalf);
		circBonusHalf->runAction(RepeatForever::create(RotateBy::create(0.15f, 45.0f)));
		auto circBonus = MAKESPRITE("circle_large");
		circBonus->setScale(0.5f);
		circBonus->setColor(Color3B::BLACK);
		bonusLayer->addChild(circBonus);
		bottomLayer->addChild(bonusLayer);
		circBonus->setName("circBonus");
		auto valMul = Label::createWithTTF("x1", NORMAL_TTF, 48.0f);
		valMul->setHorizontalAlignment(TextHAlignment::CENTER);
		//valMul->setAnchorPoint(Point(0.45f, 0.45f));
		valMul->setName("valMul");
		bonusLayer->addChild(valMul);
		auto valBonusTimer = Label::createWithBMFont(NORMAL_FNT, "0.00");
		valBonusTimer->setHorizontalAlignment(TextHAlignment::CENTER);
		valBonusTimer->setName("valBonusTimer");
		valBonusTimer->setScale(0.5f);
		valBonusTimer->setOpacity(0);
		valBonusTimer->setAnchorPoint(Point(0.5f, -0.5f));
		bonusLayer->addChild(valBonusTimer);
		
		bonusLayer->setPositionX(0.5f * (circWins->getPositionX() + circTime->getPositionX()));
		bonusLayer->setPositionY(circWins->getPositionY());
		timerLayer = valTime;
		winsLayer = valWins;
	}

	void ContrastScene::timerTick(float dt)
	{
		time--;
		if (time < 0)
		{
			showGameOver();
		}
		else
		{
			timerLayer->setString(nToString(static_cast<int>(time)) + LS("SecondSuffix"));

			if (time < 10)
			{
				timerLayer->runAction(Sequence::createWithTwoActions(
					TintTo::create(0.1f, 255, 40, 40),
					TintTo::create(0.2f, 0, 0, 0)
					));
				timerLayer->runAction(Sequence::createWithTwoActions(
					ScaleTo::create(0.1f, 1.0f),
					ScaleTo::create(0.2f, 0.75f)
					));
			}
			else if (time < 20)
			{
				timerLayer->runAction(Sequence::createWithTwoActions(
					ScaleTo::create(0.1f, 1.0f),
					ScaleTo::create(0.2f, 0.75f)
					));
			}
		}
	}
    
	void ContrastScene::bonusTick(float dt)
	{
		bool bonuswasntzero = bonustime > 0;
		bonustime -= dt;
		if (bonustime < 0)
		{
			bonustime = 0;
			bonusmultiplier = 1;

			if (bonuswasntzero)
			{
				bonusLayer->getChildByName("circHalf")->runAction(FadeOut::create(0.3f));
				bonusLayer->getChildByName("circHalf")->runAction(ScaleTo::create(0.3f, 0.1f));
				static_cast<Label*>(bonusLayer->getChildByName("valMul"))->setString(
					"x" + nToString(bonusmultiplier));
				bonusLayer->getChildByName("valBonusTimer")->runAction(FadeOut::create(0.5f));
				bonusLayer->getChildByName("valMul")->runAction(MoveTo::create(0.4f, Point::ZERO));
			}
		}

		float bonusa = bonustime / MAX_BONUS_TIME;
		bonusLayer->getChildByName("circBonus")->setColor(
			lerp(Color3B(255,0,127), Color3B(0, 200, 0), bonusa));
		double roundedbonustime = (double) ((int) (bonustime * 100)) * 0.01;
		static_cast<Label*>(bonusLayer->getChildByName("valBonusTimer"))->setString(
			nToString(roundedbonustime));
	}

    Node* ContrastScene::createButton(Color3B color, bool answer, string text)
    {
        Node* btn = nullptr;
		function<void(Ref*)> target = answer ?
			CC_CALLBACK_1(ContrastScene::win, this) :
			CC_CALLBACK_1(ContrastScene::dummy, this);

        //make button
        auto up = MAKESPRITE(block_sprite);
        auto dn = MAKESPRITE(block_sprite);
        up->setColor(color);
        dn->setColor(color);
        up->ignoreAnchorPointForPosition(false);
        dn->ignoreAnchorPointForPosition(false);
        up->setContentSize(Size(blockSize, blockSize));
        dn->setContentSize(Size(blockSize, blockSize));

		if (text.size() > 0)
		{
			auto lblup = Label::createWithTTF(text, BOLD_TTF, 20.0f);
			auto lbldn = Label::createWithTTF(text, BOLD_TTF, 20.0f);
			lblup->setPosition(Point(blockSize * 0.5f, blockSize * 0.5f));
			lbldn->setPosition(Point(blockSize * 0.5f, blockSize * 0.5f));
			up->addChild(lblup);
			dn->addChild(lbldn);
		}

        btn = MenuItemSprite::create(up, dn, target);

        return btn;
    }

	vector<Point> ContrastScene::createSquare()
	{
		vector<Point> out;
		int size = rand() % maxsize + 3;
		float gapSize = PX(CONTENT_GAP_P);

		log("createSquare, size=" + nToString(size));
		
		float x = 0;
		float y = 0;
		for (size_t iy = 0; iy < static_cast<size_t>(size); iy++)
		{
			x = 0;
			for (size_t ix = 0; ix < static_cast<size_t>(size); ix++)
			{
				out.push_back(Point(x, y));
				x += gapSize + blockSize;
			}
			y += gapSize + blockSize;
		}

		x -= gapSize + blockSize;
		y -= gapSize + blockSize;

		x *= 0.5f;
		y *= 0.5f;

		for (size_t i = 0; i < out.size(); i++)
			out[i] -= Vec2(x, y);

		return out;
	}
    
	vector<Point> ContrastScene::createBar()
    {
		vector<Point> out;

        int size = rand() % maxsize + 3;
        float gapSize = PX(CONTENT_GAP_P);
        
		int mode = rand() % 2;

        float x = 0;
        float y = 0;

		log("CreateBar, size=" + nToString(size) + ", mode=" + nToString(mode));
		if (mode == 0) // -
		{
			for (size_t ix = 0; ix < static_cast<size_t>(size); ix++)
			{
				out.push_back(Point(x, y));
				x += gapSize + blockSize;
			}
			y += gapSize + blockSize;
		}
		else // |
		{
			x = 0;
			for (size_t iy = 0; iy < static_cast<size_t>(size); iy++)
			{
				out.push_back(Point(x, y));
				y += gapSize + blockSize;
			}
			x += gapSize + blockSize;
		}

		x -= gapSize + blockSize;
		y -= gapSize + blockSize;

		x *= 0.5f;
		y *= 0.5f;

		for (size_t i = 0; i < out.size(); i++)
			out[i] -= Vec2(x, y);

		return out; 
    }

    vector<Point> ContrastScene::createPlus()
    {
        vector<Point> out;
        
        int size = rand() % maxsize + 3;
        float gapSize = PX(CONTENT_GAP_P);
        
        float x = 0;
        float y = 0;
        
        //only works on odd numbers
        size = rand() % (size * 2 - 1);
        if (size % 2 == 0) size--;
        if (size <= 1) size = 3;
        for (size_t iy = 0; iy < static_cast<size_t>(size); iy++)
        {
            x = 0;
            for (size_t ix = 0; ix < static_cast<size_t>(size); ix++)
            {
                if (iy == size / 2 || ix == size / 2)
                {
                    out.push_back(Point(x, y));
                }
                x += gapSize + blockSize;
            }
            y += gapSize + blockSize;
        }

		x -= gapSize + blockSize;
		y -= gapSize + blockSize;

		x *= 0.5f;
		y *= 0.5f;

		for (size_t i = 0; i < out.size(); i++)
			out[i] -= Vec2(x, y);

        return out;
    }
    
	Vec2 ContrastScene::createColor(vector<Point> items)
	{
		int size = static_cast<int>(items.size());
		int c = 0;
		int r = rand() % (size);

		float baseHue = RANDF * 0.6f;
		float goalHue = fmodf(baseHue + distance, 1.0f);
		Color3B baseColor = hsl2Rgb(baseHue, 0.9f, 0.5f);
		Color3B goalColor = hsl2Rgb(goalHue, 0.9f, 0.5f);

		Point o = *items.begin();

		for (auto item : items)
		{
			Node* btn = createButton(c == r ? goalColor : baseColor, c == r);
			btn->setPosition(item);
			current->addChild(btn);
			c++;

			if (item.x > o.x) o.x = item.x;
			if (item.y > o.y) o.y = item.y;
		}

		enableRotation(o);
		return o;
	}
    
	Vec2 ContrastScene::createColorWord(vector<Point> items, bool opposite)
	{
		int size = static_cast<int>(items.size());
		int c = 0;
		int r = rand() % (size);

		Point o = *items.begin();

		for (auto item : items)
		{
			int i = rand() % colors.size();
			Color3B color = colors[i];
			string label = labels[i];
			if ((c == r && !opposite) || (c != r && opposite))
			{
				do
				{
					int j = rand() % colors.size();
					if (i == j) continue;
					label = labels[j];
					break;
				}
				while (true);
			}
			Node* btn = createButton(color, c == r, LS(label));
			btn->setPosition(item);
			current->addChild(btn);
			c++;

			if (item.x > o.x) o.x = item.x;
			if (item.y > o.y) o.y = item.y;
		}

		return o;
	}
    
    Vec2 ContrastScene::createColorWordNormal(vector<Point> items)
    {
        return createColorWord(items, false);
    }
    
    Vec2 ContrastScene::createColorWordOpposite(vector<Point> items)
    {
        return createColorWord(items, true);
    }

	Vec2 ContrastScene::createColorSpell(vector<Point> items, bool opposite)
	{
		int size = static_cast<int>(items.size());
		int c = 0;
		int r = rand() % (size);

		Point o = *items.begin();

		for (auto item : items)
		{
			int i = rand() % colors.size();
			Color3B color = colors[i];
			string label = labels[i];
			if ((c == r && !opposite) || (c != r && opposite))
			{
				do
				{
					string label2 = "";
					size_t c1 = rand() % label.size();
					do
					{
						size_t c2 = rand() % label.size();
						if (c1 == c2) continue;
						for (size_t j = 0; j < label.size(); j++)
						{
							if (j == c1) label2 += nToString(label[c2]);
							else if (j == c2) label2 += nToString(label[c1]);
							else label2 += nToString(label[j]);
						}
						break;
					} while (true);
					
					if (label == label2) continue;
					label = label2;
					break;
				} while (true);
				
			}
			Node* btn = createButton(color, c == r, LS(label));
			btn->setPosition(item);
			current->addChild(btn);
			c++;

			if (item.x > o.x) o.x = item.x;
			if (item.y > o.y) o.y = item.y;
		}

		return o;
	}

	Vec2 ContrastScene::createColorSpellNormal(vector<Point> items)
	{
		return createColorSpell(items, false);
	}

	Vec2 ContrastScene::createColorSpellOpposite(vector<Point> items)
	{
		return createColorSpell(items, true);
	}

	void ContrastScene::enableRotation(Vec2 s)
	{
		//FIXME. THIS DOESN'T WORK
		if (s.length() < min(DESIGN_X, DESIGN_Y))
		{
			current->runAction(RepeatForever::create(RotateBy::create(1.0f, 10.0f * rotationDirection)));
			rotationDirection *= -1.0f;
			/*recursiveAction(current, [](Node* n) {
				n->ignoreAnchorPointForPosition(false);
				n->setAnchorPoint(Point(0.5f, 0.5f));
				n->runAction(RepeatForever::create(RotateBy::create(1.0f, -10.0f)));
			}, false);*/
		}
	}
    
    void ContrastScene::btnPausePressed(Ref*)
    {
		if (pauseCover)
		{
			removeChild(pauseCover);
			pauseCover = nullptr;
		}

#if (CC_TARGET_PLATFORM != CC_PLATFORM_ANDROID) //looks creepy on android
		static_cast<ShaderLayer*>(shader)->changeProgram(
			GLProgram::createWithFilenames("generic.vsh", "gray.glsl"));
#endif

		unschedule(schedule_selector(ContrastScene::bonusTick));
		unschedule(schedule_selector(ContrastScene::timerTick));
		unscheduleUpdate();

		PauseLayer::Pause(this, CC_CALLBACK_0(ContrastScene::resumeSchedules, this),
			[this]() { this->showGameOver(); });
    }
    
	void ContrastScene::resumeSchedules()
	{
		if (pauseCover)
		{
			removeChild(pauseCover);
			pauseCover = nullptr;
		}
#if (CC_TARGET_PLATFORM != CC_PLATFORM_ANDROID) //looks creepy on android
		static_cast<ShaderLayer*>(shader)->changeProgram(
			GLProgram::createWithFilenames("generic.vsh", "generic.psh"));
#endif
		
		bonustime = 0.01f;

		scheduleUpdate();
		schedule(schedule_selector(ContrastScene::timerTick), 1);
		schedule(schedule_selector(ContrastScene::bonusTick));
	}

    bool ContrastScene::onTouchBegan(Touch*, Event*)
    {
		return false;
    }
	
	void ContrastScene::updateLivesUI()
	{
		auto modelSize = MAKESPRITE("heart_on")->getContentSize();
		float x = -0.5f * (modelSize.width * 1.1f * (MAX_LIVES - 1));
		livesLayer->removeAllChildren();

		for (int i = 0; i < MAX_LIVES; i++)
		{
			string sprite = "heart_";
			if (i >= lives) sprite += "off";
			else sprite += "on";

			auto s = MAKESPRITE(sprite);
			if (i < lives) 
				s->setColor(Color3B::RED);
			s->setOpacity(30);
			s->setPosition(Point(x, 0));
			livesLayer->addChild(s);

			if (i == lives)
				s->runAction(Sequence::create(
				TintTo::create(0.5f, 255, 0, 0),
				TintTo::create(0.5f, 255, 255, 255),
				NULL));
			x += modelSize.width * 1.1f;
		}
	}

	Scene* IntermissionScene::createScene(Scene* _target)
	{
		_target->retain();

		Scene* scene = Scene::create();
		IntermissionScene* intermission = IntermissionScene::create();
		intermission->target = _target;
		scene->addChild(intermission);
		return scene;
	}

    bool IntermissionScene::init()
    {
        if (!Layer::init()) return false;
        
        auto bgcolor = LayerColor::create(GameInfos[Games::Contrast].color, DESIGN_X, DESIGN_Y);
        addChild(bgcolor);
        
        auto pMenu = Menu::create();
        pMenu->setPosition(Point::ZERO);
        addChild(pMenu);
        
        auto smiley = MAKESPRITE("c_pink");
        addChild(smiley);
        smiley->setPosition(CP(0.5f, 0.65f));
        smiley->setOpacity(0);
        smiley->setScale(0.01f);
        smiley->runAction(ScaleTo::create(0.4f, 1.0f));
        smiley->runAction(FadeIn::create(0.2f));
        
        auto completetext = MAKESPRITE("lc_pink");
        addChild(completetext);
        completetext->setPosition(CP(0.5f, 0.4f));
        completetext->setOpacity(0);
        completetext->setScale(0.01f);
        completetext->runAction(ScaleTo::create(0.4f, 1.0f));
        completetext->runAction(FadeIn::create(0.2f));
        
        auto score = Label::createWithTTF(
            string_format(LS("Score"), ContrastScene::LastScore), BOLD_TTF, 36.0f);
        addChild(score);
        score->setColor(Color3B::BLACK);
        score->setPosition(CP(-1.0f, 0.2f));
        score->runAction(MoveTo::create(0.7f, CP(0.5f, 0.2f)));
        
        auto loadingText = Label::createWithTTF(LS("Loading"), NORMAL_TTF, 36.0f);
        addChild(loadingText);
        loadingText->setColor(Color3B::BLACK);
        loadingText->setPosition(CP(0.5f, 0.1f));
        loadingText->setName("loadingText");
        loadingText->setOpacity(0);
        loadingText->runAction(
            Sequence::createWithTwoActions(
                FadeIn::create(1.0f),
                RepeatForever::create(
                Sequence::createWithTwoActions(
                FadeOut::create(0.4f),FadeIn::create(0.4f)))));
        
        scheduleOnce(schedule_selector(IntermissionScene::unlock), 10.0f);
        
        locked = true;
        return true;
    }

	void IntermissionScene::unlock(float)
	{
		locked = false;
		getChildByName("loadingText")->runAction(FadeOut::create(0.3f));

		auto tapText = Label::createWithTTF(LS("TapToContinue"), NORMAL_TTF, 36.0);
		addChild(tapText);
		tapText->setColor(Color3B::BLACK);
		tapText->setPosition(CP(0.5f, 0.1f));

		auto touchListener = EventListenerTouchOneByOne::create();
		touchListener->onTouchBegan = CC_CALLBACK_2(IntermissionScene::touched, this);
		getEventDispatcher()->addEventListenerWithSceneGraphPriority(touchListener, this);
	}

	bool IntermissionScene::touched(Touch*, Event*)
	{
		Director::getInstance()->replaceScene(target);
		return false;
	}
}