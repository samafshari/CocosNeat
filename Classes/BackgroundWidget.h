#ifndef __BACKGROUND_WIDGET_H__
#define __BACKGROUND_WIDGET_H__

#include <vector>
#include <map>
#include <string>
#include "cocos2d.h"
USING_NS_CC;
using namespace std;
class BackgroundItem : public Sprite
{
public:
	Point velocity;
	float angularVelocity;
	float lifeTime;
	bool faded;
	static BackgroundItem* create(string);
};

class BackgroundWidget : public Node
{
public:
	float summonTime;
	BackgroundWidget();
	Node* layer;
	static string DefaultFile;

	virtual bool init();
    static Node* create(string frame, Color3B color=Color3B::WHITE);
	static Node* createWithDefaultBackground(Color3B color = Color3B::WHITE, bool particles=true, int index=0);
	static BackgroundWidget* createWithoutBackground();
	void update(float) override;
	void createItem();

	void onAcceleration(Acceleration* accel, Event*);

	float x;
	float y;
};

#endif
