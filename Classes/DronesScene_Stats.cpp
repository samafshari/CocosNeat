#include "DronesScene.h"

using namespace Drones;

Layer* DronesScene::createGameStats(Layer* parent, float width, float height, 
	Color3B statForeColor, Color4B statBackColor)
{
	auto data = UserDefault::getInstance();
	float lblFontSize = 32.0f;
	float valFontSize = 48.0f;
	float lblY = height - lblFontSize * 0.5f;
	float valY = lblY - valFontSize;

	LayerColor* bottomLayer = LayerColor::create(statBackColor, width, height);
	parent->addChild(bottomLayer);
	bottomLayer->setPosition(Point::ZERO);

	auto lblTime = Label::createWithTTF(LS("Time"), NORMAL_TTF, lblFontSize);
	lblTime->setHorizontalAlignment(TextHAlignment::CENTER);
	lblTime->setAnchorPoint(Point(0.5f, 1.0f));
	lblTime->setPosition(Point(PX(0.25f), lblY));
	lblTime->setColor(statForeColor);
	lblTime->setTag(10);
	bottomLayer->addChild(lblTime);

	auto lblKills = Label::createWithTTF(LS("Kills"), NORMAL_TTF, lblFontSize);
	lblKills->setHorizontalAlignment(TextHAlignment::CENTER);
	lblKills->setAnchorPoint(Point(0.5f, 1.0f));
	lblKills->setPosition(Point(PX(0.5f), lblY));
	lblKills->setColor(statForeColor);
	lblKills->setTag(20);
	bottomLayer->addChild(lblKills);

	auto lblMoves = Label::createWithTTF(LS("Moves"), NORMAL_TTF, lblFontSize);
	lblMoves->setHorizontalAlignment(TextHAlignment::CENTER);
	lblMoves->setAnchorPoint(Point(0.5f, 1.0f));
	lblMoves->setPosition(Point(PX(0.75f), lblY));
	lblMoves->setColor(statForeColor);
	lblMoves->setTag(30);
	bottomLayer->addChild(lblMoves);

	auto valTime = Label::createWithBMFont(NORMAL_FNT,
		nToString(data->getIntegerForKey(("timee" + nToString(GameSettings::CurrentEpisode) + "l" + nToString(GameSettings::CurrentLevelId)).c_str(), 0)) + "s");
	valTime->setHorizontalAlignment(TextHAlignment::CENTER);
	valTime->setAnchorPoint(Point(0.5f, 1.0f));
	valTime->setPosition(Point(PX(0.25f), valY));
	valTime->setColor(statForeColor);
	valTime->setTag(11);
	bottomLayer->addChild(valTime);

	auto valKills = Label::createWithBMFont(NORMAL_FNT,
#ifdef FA
		nToString(data->getIntegerForKey(("killse" + nToString(GameSettings::CurrentEpisode) + "l" + nToString(GameSettings::CurrentLevelId)).c_str(), 0)));
#else
		nToString(data->getIntegerForKey(("killse" + nToString(GameSettings::CurrentEpisode) + "l" + nToString(GameSettings::CurrentLevelId)).c_str(), 0)) + " / " + nToString(game->monsters));
#endif
	valKills->setHorizontalAlignment(TextHAlignment::CENTER);
	valKills->setAnchorPoint(Point(0.5f, 1.0f));
	valKills->setPosition(Point(PX(0.5f), valY));
	valKills->setColor(statForeColor);
	valKills->setTag(21);
	bottomLayer->addChild(valKills);

	auto valMoves = Label::createWithBMFont(NORMAL_FNT,
		nToString(data->getIntegerForKey(("movese" + nToString(GameSettings::CurrentEpisode) + "l" + nToString(GameSettings::CurrentLevelId)).c_str(), 0)));
	valMoves->setHorizontalAlignment(TextHAlignment::CENTER);
	valMoves->setAnchorPoint(Point(0.5f, 1.0f));
	valMoves->setPosition(Point(PX(0.75f), valY));
	valMoves->setColor(statForeColor);
	valMoves->setTag(31);
	bottomLayer->addChild(valMoves);

	return bottomLayer;
}

void DronesScene::updateGameStats(float)
{
	if (dead)
	{
		unschedule(schedule_selector(DronesScene::updateGameStats));
		return;
	}

	timeval t;
	gettimeofday(&t, NULL);

	int seconds = t.tv_sec - moveBeginTime.tv_sec;
	if (game->moves == 0) seconds = 0;

	auto layer = getChildByTag(NEWSTATSTAG);
	((Label*) (layer->getChildByTag(11)))->setString(nToString(seconds) + LS("SecondSuffix"));
#ifdef FA
	((Label*) (layer->getChildByTag(21)))->setString(nToString(game->kills));
#else
	((Label*) (layer->getChildByTag(21)))->setString(nToString(game->kills) + LS("Slash") + nToString(game->monsters));
#endif
	((Label*) (layer->getChildByTag(31)))->setString(nToString(game->moves));
}

void DronesScene::saveGameStats()
{
	timeval t;
	gettimeofday(&t, NULL);

	int seconds = t.tv_sec - moveBeginTime.tv_sec;
	auto data = UserDefault::getInstance();
	int oldSeconds = data->getIntegerForKey(("timee" + nToString(GameSettings::CurrentEpisode) + "l" + nToString(GameSettings::CurrentLevelId)).c_str(), -1);
	int oldKills = data->getIntegerForKey(("killse" + nToString(GameSettings::CurrentEpisode) + "l" + nToString(GameSettings::CurrentLevelId)).c_str(), -1);
	int oldMoves = data->getIntegerForKey(("movese" + nToString(GameSettings::CurrentEpisode) + "l" + nToString(GameSettings::CurrentLevelId)).c_str(), -1);

	int newSeconds = min(oldSeconds, seconds);
	int newMoves = min(oldMoves, game->moves);
	int newKills = max(oldKills, game->kills);

	if (newSeconds < 0)
	{
		newSeconds = seconds;
		newMoves = game->moves;
		newKills = game->kills;
	}

	data->setIntegerForKey(("timee" + nToString(GameSettings::CurrentEpisode) + "l" + nToString(GameSettings::CurrentLevelId)).c_str(), newSeconds);
	data->setIntegerForKey(("killse" + nToString(GameSettings::CurrentEpisode) + "l" + nToString(GameSettings::CurrentLevelId)).c_str(), newKills);
	data->setIntegerForKey(("movese" + nToString(GameSettings::CurrentEpisode) + "l" + nToString(GameSettings::CurrentLevelId)).c_str(), newMoves);

	data->flush();
}