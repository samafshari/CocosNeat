#ifndef __VERTEX_H__
#define __VERTEX_H__

#include "cocos2d.h"
USING_NS_CC;

#define VERTEXNODE_CIRCLETAG 1
#define VERTEXNODE_LABELTAG 2
#define VERTEXNODE_RINGTAG 3

class VertexNode : public cocos2d::Layer
{
	int originalValue;
	int value;

public:
	Sprite* circle;
	Label* label;
	Sprite* ring;

	static VertexNode* create(int value);
    virtual bool init(int value=0); 

	inline int getValue() { return value; }
	inline int getOriginalValue() { return originalValue; }

	void setValue(int);
	void setOriginalValue(int);

	void shine();
	void tint(float duration, Color3B color);
};

#endif