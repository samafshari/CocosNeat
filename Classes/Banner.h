#ifndef __BANNER_H__
#define __BANNER_H__

#include "helpers.h"

using namespace std;

class Banner : public Layer
{
public:
	int chance;
	static Banner* create(vector<string>, int _chance=1);
	virtual bool init(vector<string>, int=1);
	virtual void activate();
	virtual void deactivate();
};

class HighScoreBanner : public Banner
{
public:
	CREATE_FUNC(HighScoreBanner);
	virtual bool init() override;
    bool scoresLoaded;
	string categoryToShow;
    void reset(string category);
	virtual void activate() override;
	virtual void deactivate() override;

	vector<ScorePair> oldLeaderBoard;
};

class BannerRotator : public Layer
{
    bool touchLocked;
    
	vector<Banner*> banners;
	int currentBanner;
    void changeBannerTick(float=0);
    void unlockTouch(float=0);
	void changeBanner(bool forceWipe=false);

public:
	static float BannerDuration;
	bool shuffle;

	static BannerRotator* create(vector<Banner*>, bool shuffle = true);
	virtual bool init(vector<Banner*>, bool shuffle);
    virtual bool onTouchBegan(Touch*, Ref*);
};
#endif
