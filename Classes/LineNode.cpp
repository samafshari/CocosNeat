#include "LineNode.h"
#include "helpers.h"
using namespace std;

LineNode::LineNode() 
{
	start = Point::ZERO;
	end = Point(1, 1);
	color = Color4B::GRAY;
	thickness = 1;
}

LineNode* LineNode::create(Point start, Point end, Color4B color, float thickness)
{
	LineNode* line = new LineNode();
	if (line && line->init(start, end, color, thickness))
	{
		line->autorelease();
	}
	else
	{
		delete line;
		line = NULL;
	}
	return line;
}

bool LineNode::init(Point start, Point end, Color4B color, float thickness)
{
	this->start = start;
	this->end = end;
	this->color = color;
	this->thickness = thickness;

	initLayer();
	return true;
}

void LineNode::initLayer()
{
	layer = LayerColor::create(color, 1.0f, thickness);
	addChild(layer);
	layer->ignoreAnchorPointForPosition(false);
	layer->setAnchorPoint(Point(0.0f, 0.5f));
	updateVector();
}

void LineNode::updateVector()
{
	auto vec = end - start;
	auto rad = vec.getAngle();
	auto deg = CC_RADIANS_TO_DEGREES(-rad);
	auto len = vec.getLength();
	setPosition(start);
	layer->setContentSize(Size(len, thickness));
	layer->setRotation(deg);
}

void LineNode::setStart(Point _s)
{
	start = _s;
	updateVector();
}

void LineNode::setEnd(Point _e)
{
	end = _e;
	updateVector();
}

void LineNode::setColor(Color4B _c)
{
	color = _c;
	removeChild(layer);
	initLayer();
}

void LineNode::setThickness(float _w)
{
	thickness = _w;
	//removeChild(layer);
	//initLayer();
	updateVector();
}