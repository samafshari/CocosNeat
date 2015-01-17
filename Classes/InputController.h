#ifndef __InputLayer_SCENE_H__
#define __InputLayer_SCENE_H__

#include <functional>
#include "cocos2d.h"
USING_NS_CC;

class InputController : public Layer
{
public:
	virtual void reset() {}
	virtual void kill() {}
};

#endif
