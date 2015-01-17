#include "DronesScene.h"

using namespace Drones;

Layer* DronesScene::createRandomStats(Layer* parent, float width, float height,
	Color3B statForeColor, Color4B statBackColor)
{
	auto data = UserDefault::getInstance();
	float lblFontSize = 32.0f;
	float valFontSize = 40.0f;
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

	auto lblScore = Label::createWithTTF(LS("Score"), NORMAL_TTF, lblFontSize);
	lblScore->setHorizontalAlignment(TextHAlignment::CENTER);
	lblScore->setAnchorPoint(Point(0.5f, 1.0f));
	lblScore->setPosition(Point(PX(0.75f), lblY));
	lblScore->setColor(statForeColor);
	lblScore->setTag(20);
	bottomLayer->addChild(lblScore);

	auto valTime = Label::createWithBMFont(NORMAL_FNT,
		nToString(GameSettings::LastTime) + LS("SecondSuffix"));
	valTime->setHorizontalAlignment(TextHAlignment::CENTER);
	valTime->setAnchorPoint(Point(0.5f, 1.0f));
	valTime->setPosition(Point(PX(0.25f), valY));
	valTime->setColor(statForeColor);
	valTime->setTag(11);
	bottomLayer->addChild(valTime);

	auto valScore = Label::createWithBMFont(NORMAL_FNT,
		nToString(GameSettings::DronesLastScore));
	valScore->setHorizontalAlignment(TextHAlignment::CENTER);
	valScore->setAnchorPoint(Point(0.5f, 1.0f));
	valScore->setPosition(Point(PX(0.75f), valY));
	valScore->setColor(statForeColor);
	valScore->setTag(21);
	bottomLayer->addChild(valScore);

	return bottomLayer;
}

void DronesScene::updateRandomTimer(float)
{
	if (dead)
	{
		unschedule(schedule_selector(DronesScene::updateRandomStats));
	}
	else
	{
		GameSettings::LastTime--;
		if (GameSettings::LastTime < 0) GameSettings::LastTime = 0;
	}

	if (!dead && GameSettings::LastTime == 0)
	{
		randomGameOver();
		return;
	}

	updateRandomStats();
}

void DronesScene::updateRandomStats(float)
{
	auto layer = getChildByTag(NEWSTATSTAG);
	((Label*) (layer->getChildByTag(11)))->setString(nToString(GameSettings::LastTime) + "s");
	((Label*) (layer->getChildByTag(21)))->setString(nToString(GameSettings::DronesLastScore));
}