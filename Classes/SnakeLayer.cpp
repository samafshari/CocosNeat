#include "SnakeLayer.h"
#include "SnakeScene.h"
#include "Ooze.h"
#include "ExtraGameOverScene.h"
#include "GameSettings.h"

#define OOZE_GAMESTATS \
	{ "MaximumLength", nToString(maximumLength) }, \
	{ "Score", nToString(score) }, \
	{ "Bonus", nToString(bonus) }, \
	{ "Speed", nToString(speed) }, \
	{ "ItemsPicked", nToString(itemsPicked) }

namespace Snake
{
	bool SnakeLayer::init()
	{
		maximumLength = 100;
		itemsPicked = 0;
		score = 0;
		bonus = 0;
		direction = Point::ZERO;
		speed = 180;

		LayerColor* lc = LayerColor::create(Color4B(0, 0, 0, 210), PX(1.0f), PY(0.8f));
		lc->setPosition(CP(0.5f, 0.5f));
		lc->setAnchorPoint(Point(0.5f, 0.5f));
		lc->ignoreAnchorPointForPosition(false);
		addChild(lc);
		bounds = lc->getBoundingBox();

		LayerColor* scorebg = LayerColor::create(Color4B::BLACK, PX(1.0f), PY(0.1f));
		scorebg->setPosition(Point::ZERO);
		addChild(scorebg);

		addChild(lineGroup = Node::create());
		addChild(bonusGroup = Node::create());
		addChild(itemGroup = Node::create());

		LineNode* line = LineNode::create(CP(0.5f, 0.5f), CP(0.5f, 0.5f));
		lineGroup->addChild(line);

		schedule(schedule_selector(SnakeLayer::considerSpawningBonus), 5.0f);
		scheduleUpdate();

		lblScore = Label::createWithTTF(LS("ScoreColon") + " 0", NORMAL_TTF, 42.0f);
		lblScore->setColor(Color3B::WHITE);
		lblScore->setPosition(CP(0.5f, 0.05f));
		//lblScore->enableGlow(Color4B::BLACK);
		//lblScore->enableShadow(Color4B::BLACK, Size::ZERO, 4);
		addChild(lblScore);

		string instruction = LS("SNAKE_INSTRUCTIONS");
		lblInstruction = Label::createWithTTF(instruction, NORMAL_TTF, 48.0f);
		lblInstruction->setPosition(CP(0.5f, 0.5f));
		lblInstruction->setHorizontalAlignment(TextHAlignment::CENTER);
		lblInstruction->setColor(Color3B::WHITE);
		lblInstruction->enableGlow(Color4B::RED);
		lblInstruction->runAction(RepeatForever::create(
			Sequence::createWithTwoActions(ScaleTo::create(1.0f, 1.1f),
			ScaleTo::create(1.0f, 1.0f))));
		addChild(lblInstruction);
		instructionVisible = true;

		return true;
	}

	bool SnakeLayer::move(Directions d)
	{
		Point newDirection = Point::ZERO;
		if (d == Directions::Left) newDirection.x = -1;
		if (d == Directions::Right) newDirection.x = 1;
		if (d == Directions::Up) newDirection.y = 1;
		if (d == Directions::Down) newDirection.y = -1;

		if (direction == newDirection) return false;
		if (-direction == newDirection)
		{
			return false;
			//gameOver();
		}
		else
		{
			if (instructionVisible)
			{
				spawnItem();
				instructionVisible = false;
				lblInstruction->runAction(FadeOut::create(0.5f));
			}

			direction = newDirection;
			LineNode* old = static_cast<LineNode*>(*lineGroup->getChildren().rbegin());
			LineNode* line = LineNode::create(old->getEnd(), old->getEnd(),
				Color4B::WHITE, 16.0f);
			lineGroup->addChild(line);
		}

		return true;
	}

	void SnakeLayer::update(float dt)
	{
		if (direction == Point::ZERO) return;

		drag(dt);
		truncate();
		if (testDie()) return;
		testPick();
	}

	void SnakeLayer::itemPicked()
	{
		score++;
		itemsPicked++;

		spawnItem();
		maximumLength += 100;
		lblScore->setString(LS("ScoreColon") + " " + nToString(score));
		lblScore->runAction(Sequence::createWithTwoActions(ScaleTo::create(0.3f, 1.2f),
			ScaleTo::create(0.3f, 1.0f)));
		PlayEffect("bottle_pop_2.wav");
	}

	void SnakeLayer::bonusPicked()
	{
		score += 5;
		maximumLength += 200;
		bonus++;
		if (bonus == 10)
			GameSettings::ReportAchievement(ACHIEVEMENT_SNAKE_BONUS);
		unschedule(schedule_selector(SnakeLayer::removeBonus));
		bonusGroup->removeAllChildren();
		lblScore->setString(LS("ScoreColon") + " " + nToString(score));
		lblScore->runAction(Sequence::createWithTwoActions(ScaleTo::create(0.3f, 1.2f),
			ScaleTo::create(0.3f, 1.0f)));
		PlayEffect("bottle_pop_2.wav");
	}

	void SnakeLayer::considerSpawningBonus(float)
	{
		if (rand() % 2 == 0 && bonusGroup->getChildrenCount() == 0)
			spawnBonus();
	}

	void SnakeLayer::removeBonus(float)
	{
		bonusGroup->removeAllChildren();
	}

	void SnakeLayer::spawnItem()
	{
		itemGroup->removeAllChildren();
		Sprite* item = MAKESPRITE("star");
		item->setPosition(VCP(RANDF*0.9f + 0.05f, RANDF*0.7f + 0.15f));
		item->setScale(0.85f);
		itemGroup->addChild(item);
	}

	void SnakeLayer::spawnBonus()
	{
		Sprite* item = MAKESPRITE("lolli");
		item->setPosition(VCP(RANDF*0.9f + 0.05f, RANDF*0.7f + 0.15f));
		item->setScale(0.85f);
		bonusGroup->addChild(item);

		scheduleOnce(schedule_selector(SnakeLayer::removeBonus), 6.0f);
	}

	void SnakeLayer::truncate()
	{
		float lengthSoFar = 0;
		auto itr = lineGroup->getChildren().rbegin();
		LineNode* n = nullptr;
		for (; itr != lineGroup->getChildren().rend(); itr++)
		{
			n = static_cast<LineNode*>(*itr);
			float thisLength = (n->getEnd() - n->getStart()).getLength();
			if (lengthSoFar + thisLength > maximumLength) break;
			lengthSoFar += thisLength;
		}

		if (itr == lineGroup->getChildren().rend() || !n) return;
		Point vn = n->getStart() - n->getEnd();
		vn.normalize();
		float tailLength = maximumLength - lengthSoFar;
		n->setStart(n->getEnd() + vn * tailLength);

		itr++;
		vector<Node*> nodesToDelete;
		for (; itr != lineGroup->getChildren().rend(); itr++)
			nodesToDelete.push_back(*itr);

		for (auto node : nodesToDelete) lineGroup->removeChild(node);
	}

	void SnakeLayer::drag(float dt)
	{
		float len = dt * speed;
		LineNode* n = static_cast<LineNode*>(*lineGroup->getChildren().rbegin());
		n->setEnd(n->getEnd() + direction * len);
	}

	bool SnakeLayer::testDie()
	{
		LineNode* n = static_cast<LineNode*>(*lineGroup->getChildren().rbegin());
		Point end = n->getEnd();
		if (!bounds.containsPoint(end))
		{
			gameOver();
			return true;
		}
		for (int i = 0; i < lineGroup->getChildrenCount(); i++)
		{
			LineNode* li = static_cast<LineNode*>(*(lineGroup->getChildren().begin() + i));
			for (int j = 0; j < lineGroup->getChildrenCount(); j++)
			{
				if (abs((int) i - (int) j) <= 1) continue;
				LineNode* lj = static_cast<LineNode*>(*(lineGroup->getChildren().begin() + j));
				if (linesIntersect(
					li->getStart(), li->getEnd(),
					lj->getStart(), lj->getEnd()))
				{
					gameOver();
					return true;
				}
			}
		}
		return false;
	}

	void SnakeLayer::testPick()
	{
		bool picked = false;
		int currentScore = score;

		LineNode* n = static_cast<LineNode*>(*lineGroup->getChildren().rbegin());
		Point end = n->getEnd();
		for (auto bonus : bonusGroup->getChildren())
		{
			if (end.getDistance(bonus->getPosition()) < bonus->getBoundingBox().size.width * 0.5f)
			{
				bonusPicked();
				picked = true;
				break;
			}
		}
		for (auto item : itemGroup->getChildren())
		{
			if (end.getDistance(item->getPosition()) < item->getBoundingBox().size.width * 0.5f)
			{
				itemPicked();
				picked = true;
				break;
			}
		}

		if (picked && score >= 100 && currentScore < 100)
		{
			GameSettings::ReportAchievement(ACHIEVEMENT_SCORE_SNAKE);
		}
	}

	void SnakeLayer::gameOver()
	{
		unscheduleAllSelectors();
		unscheduleUpdate();
		GameSettings::SnakeLastScore = score;
		if (GameSettings::SnakeLastScore > GameSettings::SnakeHighScore)
			GameSettings::SnakeHighScore = GameSettings::SnakeLastScore;
		GameSettings::SaveSettings();

		static_cast<SnakeScene*>(getParent())->showGameOver();
	}
}