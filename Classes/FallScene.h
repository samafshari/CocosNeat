#ifndef __FALLSCENE_H__
#define __FALLSCENE_H__

#include "helpers.h"

class FallScene : public Layer
{
public:
	virtual bool init();
	static Scene* createScene();
	CREATE_FUNC(FallScene);

	void onKeyReleased(EventKeyboard::KeyCode keyCode, Event* event) override;
	void onAcceleration(Acceleration* accel, Event*) override;
	void updateTime(float);
	void update(float) override;
	void die(Ref* = nullptr);

	Label* timeLabel;
	Sprite* ball;
	Node* rects;

	float dx;
	float ballSpeed;
	float speed;
	float genAlpha;
	float genTime;
	
	int time;

	PhysicsWorld* world;
};

#endif
