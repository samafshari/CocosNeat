#include "helpers.h"
#include "CreditsScene.h"
#include "MainMenuScene.h"

USING_NS_CC;

void CreditsScene::ShowCredits(string filename)
{
	Scene* scene = Director::getInstance()->getRunningScene();
	scene->retain();
	Scene *creditsScene = Scene::create();
	CreditsScene *creditsLayer = CreditsScene::create();
	creditsLayer->previousScene = scene;
	creditsLayer->createBody(filename);
	creditsScene->addChild(creditsLayer);
	
	Director::getInstance()->replaceScene(TransitionSlideInB::create(1.0f, creditsScene));
}

Scene* CreditsScene::createScene(string filename)
{
    Scene *scene = Scene::create();
    CreditsScene *layer = CreditsScene::create();
	layer->createBody(filename);
    scene->addChild(layer);
    return scene;
}

static Size visibleSize;
static Point origin;

bool CreditsScene::init()
{
    if ( !Layer::init() )
    {
        return false;
    }
    
	return true;
}

void CreditsScene::createBody(string filename)
{
	addChild(LayerColor::create(Color4B(10, 10, 10, 255), DESIGN_X, DESIGN_Y));

	text = Label::createWithBMFont(
		FIXED_FNT_ENGLISH,
		FileUtils::getInstance()->getStringFromFile(filename));

	text->setAnchorPoint(Point(0.5f, 1.0f));
	text->setPosition(CP(0.5f, 1.0f));
	text->setScale(0.5f);
	text->setColor(Color3B::WHITE);
	text->setHorizontalAlignment(TextHAlignment::CENTER);
	text->setVerticalAlignment(TextVAlignment::TOP);
	addChild(text);


#if (CC_TARGET_PLATFORM != CC_PLATFORM_ANDROID)
	auto menu = Menu::create();
	menu->setPosition(Point::ZERO);
	addChild(menu);

	auto btnBackUp = LayerColor::create(Color4B(100, 5, 5, 255), DESIGN_X, PY(0.08f));
	auto btnBackUpSprite = MAKESPRITE("btnDownArrowUp");
	btnBackUpSprite->setPosition(CP(0.5f, 0.04f));
	btnBackUp->addChild(btnBackUpSprite);

	auto btnBackDn = LayerColor::create(Color4B(50, 5, 5, 255), DESIGN_X, PY(0.08f));
	auto btnBackDnSprite = MAKESPRITE("btnDownArrowUp");
	btnBackDnSprite->setPosition(CP(0.5f, 0.04f));
	btnBackDn->addChild(btnBackDnSprite);

	auto btnBack = MenuItemSprite::create(btnBackUp, btnBackDn, CC_CALLBACK_1(CreditsScene::menuCloseCallback, this));
	btnBack->setAnchorPoint(Point::ZERO);
	menu->addChild(btnBack);
#endif

	auto keyboard = EventListenerKeyboard::create();
	keyboard->onKeyReleased = CC_CALLBACK_2(CreditsScene::onKeyReleased, this);
	getEventDispatcher()->addEventListenerWithSceneGraphPriority(keyboard, this);

	if (text->getBoundingBox().size.height > DESIGN_Y)
	{
		auto touchListener = EventListenerTouchOneByOne::create();
		touchListener->onTouchBegan = CC_CALLBACK_2(CreditsScene::onTouchBegan, this);
		touchListener->onTouchMoved = CC_CALLBACK_2(CreditsScene::onTouchMoved, this);
		getEventDispatcher()->addEventListenerWithSceneGraphPriority(touchListener, this);
	}
}

void CreditsScene::menuCloseCallback(Ref*)
{
	text->runAction(MoveTo::create(0.3f, CP(0.5f, 1.0f)));
	Director::getInstance()->replaceScene(
		TransitionSlideInT::create(1.0f, previousScene));
}

void CreditsScene::onKeyReleased(EventKeyboard::KeyCode keyCode, Event* event)
{
	if (keyCode == EventKeyboard::KeyCode::KEY_ESCAPE)
		menuCloseCallback(nullptr);
}

bool CreditsScene::onTouchBegan(Touch*, Event*)
{
	return true;
}

void CreditsScene::onTouchMoved(Touch* t, Event*)
{
	text->setPositionY(text->getPositionY() + t->getDelta().y);
	auto aabb = text->getBoundingBox();
	if (text->getPositionY() < VPY(1.0f)) text->setPositionY(VPY(1.0f));
	if (aabb.getMinY() > VPY(0.1f)) text->setPositionY(VPY(0.1f) + aabb.size.height);
}