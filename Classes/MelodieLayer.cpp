#include "MelodieLayer.h"
#include "MelodieScene.h"
#include "Ooze.h"
#include "ExtraGameOverScene.h"

namespace Melodie
{
	bool MelodieLayer::init()
	{
		if (!Node::init()) return false;

		score = 0;
		lives = MAX_LIVES;
		state = GameStates::Idle;

		circle = MAKESPRITE("bigcircle");
		circle->setPosition(CP(0.5f, 0.3f));
		addChild(circle);

		stateLabel = Label::createWithTTF(LS("Wait"), NORMAL_TTF, 32.0f);
		stateLabel->setHorizontalAlignment(TextHAlignment::CENTER);
		stateLabel->setPosition(CP(0.5f, 0.33f));
		stateLabel->setAnchorPoint(Point(0.5f, 0.5f));
		stateLabel->setColor(Color3B::BLACK);
		addChild(stateLabel);

		scoreLabel = Label::createWithBMFont(NORMAL_FNT, "");
		scoreLabel->setHorizontalAlignment(TextHAlignment::CENTER);
		scoreLabel->setPosition(CP(0.5f, 0.27f));
		scoreLabel->setAnchorPoint(Point(0.5f, 0.5f));
		scoreLabel->setColor(Color3B::BLACK);
		addChild(scoreLabel);
		giveScore(0);

		livesLayer = Node::create();
		livesLayer->setPosition(CP(0.5f, 0.6f));
		addChild(livesLayer);
		updateLivesUI();

		vector<Color4B> colors =
		{
			Color4B::WHITE,
			Color4B(239, 117, 178, 255),
			Color4B(255, 252, 101, 255),
			Color4B(255, 101, 135, 255),
			Color4B(101, 255, 130, 255),
			Color4B(101, 196, 255, 255),
			Color4B(191, 101, 255, 255),
			Color4B(101, 255, 237, 255),
			Color4B(101, 145, 255, 255),
			Color4B(255, 178, 101, 255),
			Color4B(156, 255, 81, 255)
		};
		vector<Size> sizes =
		{
			Size::ZERO,
			Size(320, 320),
			Size(160, 160),
			Size(160, 160),
			Size(160, 160),
			Size(160, 320),
			Size(160, 320),
			Size(320, 160),
			Size(160, 320),
			Size(320, 320),
			Size(160, 160)
		};

		for (int i = 1; i <= 10; i++)
		{
			lit[i] = LayerColor::create(colors[i], sizes[i].width, sizes[i].height);
			lit[i]->ignoreAnchorPointForPosition(false);
			lit[i]->setAnchorPoint(Point(0, 0));
			addChild(lit[i]);

			lit[i]->setOpacity(0);
		}

		lit[1]->setPosition(0, 0);
		lit[2]->setPosition(320, 0);
		lit[3]->setPosition(480, 0);
		lit[4]->setPosition(320, 160);
		lit[5]->setPosition(480, 160);
		lit[6]->setPosition(0, 320);
		lit[7]->setPosition(160, 320);
		lit[8]->setPosition(160, 480);
		lit[9]->setPosition(320, 480);
		lit[10]->setPosition(0, 640);

		bigText = Label::createWithTTF("", NORMAL_TTF, 200.0f);
		bigText->setHorizontalAlignment(TextHAlignment::CENTER);
		bigText->setVerticalAlignment(TextVAlignment::CENTER);
		bigText->setPosition(CP(0.5f, 0.5f));
		addChild(bigText, 10);

		auto touchListener = EventListenerTouchOneByOne::create();
		touchListener->onTouchBegan = CC_CALLBACK_2(MelodieLayer::onTouchBegan, this);
		getEventDispatcher()->addEventListenerWithSceneGraphPriority(touchListener, this);

		scheduleOnce(schedule_selector(MelodieLayer::begin), 1.0f);
		return true;
	}

	void MelodieLayer::begin(float)
	{
		makeNextMove();
	}

	void MelodieLayer::flash(int i)
	{
		lit[i]->stopAllActions();
		lit[i]->runAction(Sequence::create(
			FadeOut::create(0.05f),
			FadeIn::create(0.05f),
			DelayTime::create(max(0.5f * FLASH_DURATION, FLASH_DURATION - score * 0.02f)),
			FadeOut::create(0.5f),
			NULL));

		PlayEffect("piano_m.wav", (float) i / (float) BUTTONS_COUNT);
	}

	void MelodieLayer::play()
	{
		circle->stopAllActions();
		
		cursor = 0;
		state = GameStates::Playback;
		stateLabel->setString(LS("Wait"));
		schedule(schedule_selector(MelodieLayer::play), max(PLAYBACK_PERIOD * 0.5f, PLAYBACK_PERIOD - score * 0.02f));
	}

	void MelodieLayer::play(float)
	{
		if (state != GameStates::Playback) return;
		if (cursor == static_cast<int>(seq.size()))
		{
			cursor = 0;
			unschedule(schedule_selector(MelodieLayer::play));
			schedule(schedule_selector(MelodieLayer::gameOver), TRY_TIME);
			state = GameStates::Input;
			stateLabel->setString(LS("Touch"));
			return;
		}
		unschedule(schedule_selector(MelodieLayer::gameOver));
		flash(seq[cursor++]);
	}

	void MelodieLayer::giveScore(int d)
	{
		score += d;
		scoreLabel->setString(nToString(score));
		if (d == 0) return;
		
		scoreLabel->runAction(Sequence::createWithTwoActions(
			ScaleTo::create(0.2f, 1.1f),
			ScaleTo::create(0.2f, 1.0f)));
	}

	void MelodieLayer::correct()
	{
		log("Correct.");
		giveScore(1);
		circle->runAction(Sequence::create(
			TintTo::create(0.3f, 150, 150, 255),
			TintTo::create(1.3f, 255, 255, 255),
			NULL));
		makeNextMove();
	}

	void MelodieLayer::wrong()
	{
		log("Wrong.");
		PlayEffect("wrong.wav");
		lives--;
		updateLivesUI();
		if (lives)
		{
			play();
			circle->runAction(Sequence::create(
				TintTo::create(0.3f, 255, 150, 150),
				TintTo::create(1.3f, 255, 255, 255),
				NULL));
		}
		else
		{
			gameOver();
		}
	}

	void MelodieLayer::flashBigText(string s, Color3B c)
	{
		bigText->setString(s);
		bigText->setColor(c);
		bigText->stopAllActions();
		bigText->setOpacity(255);
		bigText->runAction(Sequence::createWithTwoActions(
			DelayTime::create(0.7f),
			FadeOut::create(0.3f)));
	}

	void MelodieLayer::makeNextMove()
	{
		int next = rand() % BUTTONS_COUNT + 1;
		seq.push_back(next);
		play();
	}

	bool MelodieLayer::onTouchBegan(Touch* t, Event*)
	{
		if (state == GameStates::Playback)
		{
			flashBigText(LS("WaitBig"), Color3B::RED);
			PlayEffect("wrong.wav");
		}
		if (state != GameStates::Input) return false;
		bool hit = false;
		for (auto l : lit)
		{
			auto aabb = static_cast<Node*>(l.second)->getBoundingBox();
			if (aabb.containsPoint(t->getLocation()))
			{
				hit = true;
				int id = l.first;
				if (seq[cursor] == id)
				{
					flash(id);
					cursor++;
					log(nToString(id));

					if (cursor == static_cast<int>(seq.size()))	correct();
					return false;
				}
			}
		}
		
		if (hit) wrong();
		return false;
	}

	void MelodieLayer::gameOver(float)
	{
		log("GameOver");
		static_cast<MelodieScene*>(this->getParent())->showGameOver();
	}

	void MelodieLayer::updateLivesUI()
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
			//s->setAnchorPoint(Point(1.0f, 0.5f));
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
}