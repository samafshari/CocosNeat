#ifndef __DONKEYSCENE_H__
#define __DONKEYSCENE_H__

#include "helpers.h"

class DonkeyScene : public Layer
{
public:
	virtual bool init();
	static Scene* createScene();
	CREATE_FUNC(DonkeyScene);

	void onKeyReleased(EventKeyboard::KeyCode keyCode, Event* event) override;
	void onAcceleration(Acceleration* accel, Event*) override;
	void update(float) override;
	void die(Ref* = nullptr);

};

#endif
