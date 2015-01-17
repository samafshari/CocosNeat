#include "Vertex.h"
#include "helpers.h"

VertexNode* VertexNode::create(int value)
{
	VertexNode* pRet = new VertexNode();
	if (pRet && pRet->init(value))
	{
		pRet->autorelease();
	}
	else
	{
		delete pRet;
		pRet = NULL;
	}

	return pRet;
}

bool VertexNode::init(int _value)
{
    if ( !Layer::init() )
    {
        return false;
    }

	originalValue = _value;
	value = _value;

	ring = MAKESPRITE("ring");
	ring->setAnchorPoint(Point(0.5f, 0.5f));
	ring->setPosition(Point::ZERO);
	ring->setTag(VERTEXNODE_RINGTAG);
	ring->setScale(0.1f);
	addChild(ring);

	circle = MAKESPRITE("vertex");
	circle->setAnchorPoint(Point(0.5f, 0.5f));
	circle->setPosition(Point::ZERO);
	circle->setTag(VERTEXNODE_CIRCLETAG);
    circle->setScale(1.5f);
	addChild(circle);

	label = Label::createWithBMFont(NORMAL_FNT, nToString(_value));
	label->setAnchorPoint(Point(0.5f, 0.5f));
	label->setHorizontalAlignment(TextHAlignment::CENTER);
	label->setVerticalAlignment(TextVAlignment::CENTER);
	label->setPosition(Point::ZERO);
	label->setColor(Color3B::BLACK);
	label->setTag(VERTEXNODE_LABELTAG);
	addChild(label);

    return true;
}

void VertexNode::setValue(int _value)
{
	value = _value;
	label->setString(nToString(value));
}

void VertexNode::setOriginalValue(int _value)
{
	originalValue = _value;
}

void VertexNode::shine()
{
	ring->setScale(0.5f);
	ring->setOpacity(255);
	ring->runAction(Sequence::create(ScaleTo::create(0.25f, 1.5f), FadeOut::create(0.3f), NULL));
}

void VertexNode::tint(float duration, Color3B color)
{
	//circle->runAction(TintTo::create(duration, color.r, color.g, color.b));
}