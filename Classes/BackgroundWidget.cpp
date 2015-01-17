#include "BackgroundWidget.h"
#include "helpers.h"
using namespace std;

string BackgroundWidget::DefaultFile = "tile%d.png";

BackgroundItem* BackgroundItem::create(string frameName)
{
	BackgroundItem* item = new BackgroundItem();
	if (item && item->initWithSpriteFrameName(frameName))
	{
		item->autorelease();
		return item;
	}
	CC_SAFE_DELETE(item);
	return nullptr;
}

BackgroundWidget::BackgroundWidget()
{
}

BackgroundWidget* BackgroundWidget::createWithoutBackground()
{
	BackgroundWidget* bg = new BackgroundWidget();
	if (bg && bg->init())
	{
		bg->autorelease();
	}
	else
	{
		delete bg;
		bg = NULL;
	}
    
	return bg;
}

Node* BackgroundWidget::create(string frame, Color3B color)
{
    Node* result = Node::create();
	auto bgcolor = MAKESPRITE(frame);
    auto bgsize = bgcolor->getContentSize();
	bgcolor->setPosition(CP(0.5f, 0.5f));
	bgcolor->setAnchorPoint(Point(0.5f, 0.5f));
    bgcolor->setScaleX(DESIGN_X / bgsize.width);
    bgcolor->setScaleY(DESIGN_Y / bgsize.height);
	bgcolor->setColor(color);
	result->addChild(bgcolor);
	bgcolor->setOpacity(0);
	bgcolor->runAction(
                       Sequence::create(
                                        MoveBy::create(0.1f, Point::ZERO),
                                        FadeIn::create(0.5f),
                                        MoveBy::create(3.0f, Point::ZERO),
                                        NULL));
    result->addChild(createWithoutBackground());
    return result;
}

Node* BackgroundWidget::createWithDefaultBackground(Color3B color, bool particles, int index)
{
	Node* result = Node::create();
#if !defined(BURGERMAN)
    return create("bg");
#else
	Texture2D::TexParams texParams;
	texParams.magFilter = GL_LINEAR;
	texParams.minFilter = GL_LINEAR;
	texParams.wrapS = GL_REPEAT;
	texParams.wrapT = GL_REPEAT;
	auto bgcolor = Sprite::create(string_format(DefaultFile, index), Rect(0, 0, DESIGN_X, DESIGN_Y));
	((Sprite*) bgcolor)->getTexture()->setTexParameters(texParams);
	bgcolor->setPosition(CP(0.5f, 0.5f));
#endif
/*
	bgcolor->setAnchorPoint(Point(0.5f, 0.5f));
	bgcolor->setColor(color);
	result->addChild(bgcolor);
	bgcolor->setOpacity(0);
	bgcolor->runAction(
		Sequence::create(
		MoveBy::create(0.1f, Point::ZERO),
		FadeIn::create(0.5f),
		MoveBy::create(3.0f, Point::ZERO),
		NULL));
*/
	if (particles) result->addChild(createWithoutBackground());

	return result;
}

bool BackgroundWidget::init()
{
	layer = Node::create(); // LayerColor::create(Color4B::BLACK, FRAME_SIZE.width, FRAME_SIZE.height);
	addChild(layer);
	summonTime = 0;
	for (size_t i = 0; i < 7; i++)
	{
		createItem();
	}

	scheduleUpdate();

	auto listener = EventListenerAcceleration::create(CC_CALLBACK_2(BackgroundWidget::onAcceleration, this));
	getEventDispatcher()->addEventListenerWithSceneGraphPriority(listener, this);

	x = y = 0;
	return true;
}

void BackgroundWidget::createItem()
{
	BackgroundItem* item = BackgroundItem::create("bw" + nToString(rand() % 8));
	if (RANDF < 0.5f) item->angularVelocity = 360 / (1 + rand() % 4);
	else item->angularVelocity = RANDF * 30;
	float maxSpeed = 0.05f;
	item->velocity.x = RANDF * PX(maxSpeed * 2.0f) - PX(maxSpeed);
	item->velocity.y = RANDF * PY(maxSpeed * 2.0f) - PY(maxSpeed);
	item->setPosition(CP(RANDF, RANDF));
	item->setOpacity(0);
	//item->setScaleX(RANDF + 0.25f);
	//item->setScaleY(RANDF + 0.25f);
	item->setScale(RANDF * 1.1f + 0.1f);
	if (RANDF < 0.5f) item->runAction(ScaleBy::create(RANDF * 10.0f + 1.0f, RANDF * 4.0f + 0.5f));
	float chrome = (int)(RANDF * 70.0f);
	item->setColor(Color3B(chrome, chrome, chrome));
	item->runAction(FadeIn::create(RANDF * 5.0f + 0.3f));
	item->lifeTime = RANDF * 15.0f;
	item->faded = false;
	layer->addChild(item);
	summonTime = 0;
}

void BackgroundWidget::onAcceleration(Acceleration* accel, Event*)
{
	float d = 0.15f;
	x = -PX(d) * accel->x;
	y = -PY(d) * accel->y;
}

void BackgroundWidget::update(float dt)
{
	layer->setPositionX(lerp(layer->getPositionX(), x, 0.1f));
	layer->setPositionY(lerp(layer->getPositionY(), y, 0.1f));

	summonTime += dt;
	if (summonTime > 1.0f) createItem();

	vector<Node*> removeQueue;
	for (auto node : layer->getChildren())
	{
		BackgroundItem* item = dynamic_cast<BackgroundItem*>(node);
		if (!item) continue;
		if (item->getPositionX() < -200 || item->getPositionX() > DESIGN_X * 1.5f ||
			item->getPositionY() < -200 || item->getPositionY() > DESIGN_Y * 1.5f)
		{
			removeQueue.push_back(item);
			continue;
		}
		item->setPosition(item->getPosition() + item->velocity * dt);
		item->setRotation(item->getRotation() + item->angularVelocity * dt);
		item->lifeTime -= dt;
		if (item->lifeTime < 0 && !item->faded)
		{
			item->faded = true;
			item->runAction(FadeOut::create(RANDF * 4.0f));
		}
	}

	for (auto itr = removeQueue.begin(); itr != removeQueue.end(); itr++)
	{
		layer->removeChild(*itr);
		createItem();
	}
}