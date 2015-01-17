#ifndef __LINENODE_H__
#define __LINENODE_H__

#include <vector>
#include <map>
#include <string>
#include "cocos2d.h"
USING_NS_CC;

class VertexNode;

class LineNode : public Node
{
protected:
	Point start;
	Point end;
	Color4B color;
	float thickness;
	void initLayer();
	void updateVector();
	LayerColor* layer;

public:
	LineNode();

	virtual bool init(Point start, Point end, Color4B color, float thickness);
	static LineNode* create(Point start = Point::ZERO, Point end = Point::ZERO, Color4B color = Color4B::WHITE, float width = 1.0f);

	inline Point getStart() { return start; }
	inline Point getEnd() { return end; }
	inline Color4B getColor() { return color; }
	inline float getThickness() { return thickness; }

	void setStart(Point _s);
	void setEnd(Point _e);
	void setColor(Color4B _c);
	void setThickness(float _w);

	VertexNode *tag1, *tag2;
	int value;
};

#endif
