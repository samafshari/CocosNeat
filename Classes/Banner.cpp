#include "Banner.h"
#include "helpers.h"

Banner* Banner::create(vector<string> text, int _chance)
{
	Banner* b = new Banner();
	if (b && b->init(text, _chance))
	{
		b->autorelease();
	}
	else
	{
		delete b;
		b = NULL;
	}
	return b;
}

bool Banner::init(vector<string> text, int _chance)
{
	ignoreAnchorPointForPosition(false);
	int c = text.size() / 2;
	chance = _chance;
	int i = 0;
	removeAllChildren();
	for (auto item : text)
	{
		bool isNumber = true;
		for (char c : item)
		{
			if (c >= '0' && c <= '9')
			{

			}
			else isNumber = false;
		}
		Label* label;
		//if (isNumber) label = Label::createWithBMFont(NORMAL_FNT, item); 
		//else 
			label = Label::createWithTTF(item, NORMAL_TTF, 40.0f);
		c--;

		if (text.size() == (size_t) 1)
		{
			label->setVerticalAlignment(TextVAlignment::CENTER);
			label->setPosition(CP(0.5f, 0));
		}
		else
		{
			label->setAnchorPoint(Point(0.5f, 0));
			label->setVerticalAlignment(TextVAlignment::BOTTOM);
			label->setPosition(Point(32.0f, c * 1.3f * 40.0f));
		}
		label->setHorizontalAlignment(TextHAlignment::CENTER);
		label->setOpacity(0);
		label->setTag(i++);
		addChild(label);
	}

	return true;
}

void Banner::activate()
{
	float i = 0;
	for (auto item : getChildren())
	{
		i++;
		item->setPositionX(32);
		item->runAction(Sequence::createWithTwoActions(DelayTime::create(i * 0.35f), FadeIn::create(1.0f)));
		item->runAction(Sequence::createWithTwoActions(DelayTime::create(i * 0.35f), MoveTo::create(1.0f, Point(0, item->getPositionY()))));
	}
}

void Banner::deactivate()
{
	float i = 0;
	for (auto item : getChildren())
	{
		i++;
		item->runAction(Sequence::createWithTwoActions(DelayTime::create(i * 0.25f), FadeOut::create(3.0f)));
	}
}

bool HighScoreBanner::init()
{
    scoresLoaded = false;
    GameSettings::GameCenterDataReceived = CC_CALLBACK_1(HighScoreBanner::reset, this);
	ignoreAnchorPointForPosition(false);
	auto label = Label::createWithTTF(
		string_format("%s\n \n%s",
		LS("TXT_HIGHSCORES").c_str(),
		LS("TXT_ERROR_GAMECENTERCONNECTION").c_str()), NORMAL_TTF, 40.0f);
	label->setAnchorPoint(Point(0.5f, 0.5f));
	label->setVerticalAlignment(TextVAlignment::CENTER);
	label->setHorizontalAlignment(TextHAlignment::CENTER);
	label->setPosition(Point(32.0f, 0));
	label->setOpacity(0);
#if LIGHT_THEME
    label->setColor(Color3B::BLACK);
#endif
	addChild(label);

    if (!GameSettings::GetLeaderBoard(categoryToShow).empty()) reset(categoryToShow);
	
    return true;
}

void HighScoreBanner::reset(string category)
{
	if (category != categoryToShow)
	{
		log(string_format("Leaderboard %s recieved, but banner is for %s. Ignoring.", 
			category.c_str(), categoryToShow.c_str()));
		return;
	}
    /*
     //this doesn't work anyway
    if (!this->getParent())
    {
        GameSettings::GameCenterDataReceived = nullptr;
        return;
    }*/
	oldLeaderBoard = GameSettings::GetLeaderBoard(category);

    removeAllChildren();
	if (oldLeaderBoard.empty())
    {
        auto label = Label::createWithTTF(LS("LeaderboardNotAvailable"), NORMAL_TTF, 40.0f);
        label->setAnchorPoint(Point(0.5f, 0));
        label->setVerticalAlignment(TextVAlignment::BOTTOM);
        label->setHorizontalAlignment(TextHAlignment::CENTER);
        label->setPosition(Point(32.0f, 0));
        label->setOpacity(0);
#if LIGHT_THEME
        label->setColor(Color3B::BLACK);
#endif
        addChild(label);
        return;
    }
    
	int c = (2 + min(5, (int) oldLeaderBoard.size())) / 2;
    
	ignoreAnchorPointForPosition(false);
	auto label = Label::createWithTTF(LS("TXT_HIGHSCORES"), NORMAL_TTF, 40.0f);
	c--;
    
	label->setAnchorPoint(Point(0.5f, 0));
	label->setVerticalAlignment(TextVAlignment::BOTTOM);
	label->setHorizontalAlignment(TextHAlignment::CENTER);
	label->setPosition(Point(32.0f, c * 1.3f * 40.0f));
	label->setOpacity(0);
#if LIGHT_THEME
    label->setColor(Color3B::BLACK);
#endif
	addChild(label);
    int i=0;
	for (auto item : oldLeaderBoard)
	{
        i++;
        if (i==6) break;
		auto layer = Layer::create();
		addChild(layer);
        
		c--;
        
		auto label = Label::createWithTTF(GameSettings::GetPlayerName(item.Name), NORMAL_TTF, 40.0f);
		label->setPositionX(-PX(0.4f));
		label->setPositionY(0);
		label->setAnchorPoint(Point(0, 0));
		label->setVerticalAlignment(TextVAlignment::BOTTOM);
		label->setHorizontalAlignment(TextHAlignment::LEFT);
		label->setOpacity(0);
#if LIGHT_THEME
        label->setColor(Color3B::BLACK);
#endif
		layer->addChild(label);
        
		auto score = //Label::createWithBMFont(NORMAL_FNT, nToString(item.Score));
			Label::createWithTTF(nToString(item.TotalScore), NORMAL_TTF, 40.0f);
		score->setPositionX(PX(0.4f));
		score->setPositionY(0);
		score->setAnchorPoint(Point(1, 0));
		score->setVerticalAlignment(TextVAlignment::BOTTOM);
		score->setHorizontalAlignment(TextHAlignment::RIGHT);
		score->setOpacity(0);
#if LIGHT_THEME
        score->setColor(Color3B::BLACK);
#endif
		layer->addChild(score);
        
#if defined(FA)
		label->setAnchorPoint(Point(1, 0));
		score->setAnchorPoint(Point(0, 0));

		label->setHorizontalAlignment(TextHAlignment::RIGHT);
		score->setHorizontalAlignment(TextHAlignment::LEFT);

		auto labelPos = label->getPositionX();
		label->setPositionX(score->getPositionX());
		score->setPositionX(labelPos);
#endif
		//layer->ignoreAnchorPointForPosition(false);
        
		layer->setPosition(Point(32.0f, c * 1.3f * 40.0f));
	}
}

void HighScoreBanner::activate()
{
	auto lb = GameSettings::GetLeaderBoard(categoryToShow);
    if ((!scoresLoaded && !lb.empty()) ||
		lb != oldLeaderBoard)
    {
        scoresLoaded = true;
        reset(categoryToShow);
    }
	float i = 0;
	for (auto layer : getChildren())
	{
		i++;
		layer->setPositionX(32);
		layer->runAction(Sequence::createWithTwoActions(DelayTime::create(i * 0.35f), MoveTo::create(1.0f, Point(0, layer->getPositionY()))));
		if (i==1) layer->runAction(Sequence::createWithTwoActions(DelayTime::create(i * 0.35f), FadeIn::create(1.0f)));
		for (auto item : layer->getChildren())
		{
            item->stopAllActions();
            item->setOpacity(0);
			item->runAction(Sequence::createWithTwoActions(DelayTime::create(i * 0.35f), FadeIn::create(1.0f)));
		}
	}
}

void HighScoreBanner::deactivate()
{
	float i = 0;
	for (auto layer : getChildren())
	{
		i++;
		if (i == 1) layer->runAction(Sequence::createWithTwoActions(DelayTime::create(i * 0.25f), FadeOut::create(3.0f)));
		for (auto item : layer->getChildren())
		{
            item->stopAllActions();
			item->runAction(Sequence::createWithTwoActions(DelayTime::create(i * 0.25f), FadeOut::create(3.0f)));
		}
	}
}

float BannerRotator::BannerDuration = 7.5f;
BannerRotator* BannerRotator::create(vector<Banner*> banners, bool _shuffle)
{
	BannerRotator* b = new BannerRotator();
	if (b && b->init(banners, _shuffle))
	{
		b->autorelease();
	}
	else
	{
		delete b;
		b = NULL;
	}
	return b;
}

bool BannerRotator::init(vector<Banner*> _banners, bool _shuffle)
{
	banners = _banners;
	shuffle = _shuffle;

	int c = 0;
	for (auto banner : banners)
	{
		banner->setAnchorPoint(Point(0.5f, 0.5f));
		banner->setPosition(CP(0.5f + c++ * 1.0f, 0.5f));
		addChild(banner);
	}
	currentBanner = 0;
	banners[currentBanner]->activate();

	schedule(schedule_selector(BannerRotator::changeBannerTick), BannerDuration);
    
	auto touch = EventListenerTouchOneByOne::create();
	touch->onTouchBegan = CC_CALLBACK_2(BannerRotator::onTouchBegan, this);
	getEventDispatcher()->addEventListenerWithSceneGraphPriority(touch, this);

    touchLocked = false;
	return true;
}

bool BannerRotator::onTouchBegan(cocos2d::Touch *, cocos2d::Object *)
{
	return false; //disable

    if (touchLocked) return false;
    touchLocked = true;
    scheduleOnce(schedule_selector(BannerRotator::unlockTouch), 2.0f);
	
    unschedule(schedule_selector(BannerRotator::changeBannerTick));
	schedule(schedule_selector(BannerRotator::changeBannerTick), BannerDuration);
    //banners[currentBanner]->setPositionX(PX(-1.5f));
	for (auto banner : banners) banner->setPositionX(PX(-1.5f));
    changeBanner();
	return false;
}

void BannerRotator::unlockTouch(float)
{
    touchLocked = false;
}

void BannerRotator::changeBannerTick(float)
{
    changeBanner(false);
}

void BannerRotator::changeBanner(bool forceWipe)
{
    if (!forceWipe)
    {
        banners[currentBanner]->runAction(MoveTo::create(2.25f, Point(PX(-1.5f), banners[currentBanner]->getPositionY())));
        banners[currentBanner]->deactivate();
    }
	if (shuffle)
	{
		int oldBanner = currentBanner;
		vector<size_t> roulette;
		for (size_t i = 0; i < banners.size(); i++)
		{
			for (int j = 0; j < banners[i]->chance; j++)
				roulette.push_back(i);
		}
		do
		{
			currentBanner = roulette[rand() % roulette.size()];
			if (oldBanner == currentBanner) continue;
			break;
		} while (true);
	}
	else currentBanner = (currentBanner + 1) % banners.size();
	
	banners[currentBanner]->setPositionX(PX(0.5f));
	banners[currentBanner]->activate();
}
