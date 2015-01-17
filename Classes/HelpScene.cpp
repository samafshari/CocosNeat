#include "helpers.h"
#include "HelpScene.h"
#include "MainMenuScene.h"
#include "BackgroundWidget.h"
#include "Widgets.h"

#define SPRITEREF getChildByTag(1)->getChildByTag(0)
Scene* HelpScene::scene()
{
    Scene *scene = Scene::create();
	HelpScene *layer = HelpScene::create();
    scene->addChild(layer);
    return scene;
}

static Size visibleSize;
static Point origin;

static inline Label* makeText(string text, float size=28.0f)
{
	auto lbl = Label::createWithTTF(text, NORMAL_TTF, size);
	lbl->setPositionX(10);
	lbl->setAnchorPoint(Point(0, 1));
	lbl->setHorizontalAlignment(TextHAlignment::LEFT);
	lbl->enableShadow(Color4B::BLACK, Size(0.5f, -1.0f), 1);
	return lbl;
}

static inline Layer* makeItem(string sprite, string text, char code, bool numbered=false)
{
	Layer* l = Layer::create();
	Layer* s = Layer::create();
	auto sp = MAKESPRITE(sprite);
	l->addChild(s);
	s->addChild(sp);
	s->setPosition(Point(-64, -64));
	auto t = makeText(text);
	s->setTag(1);
	
	t->setTag(2);
	l->addChild(t);

	auto spriteSize = sp->getContentSize();
	auto spriteMidPoint = Point(
		spriteSize.width * 0.5f,
		spriteSize.height * 0.5f);
	sp->setPosition(spriteMidPoint);
	sp->setTag(0);

	if (numbered)
	{
		auto num = Label::createWithBMFont(MATH_FNT, "--");
		sp->addChild(num);
		num->setHorizontalAlignment(TextHAlignment::CENTER);
		num->setVerticalAlignment(TextVAlignment::BOTTOM);
		num->setPosition(Point(32, 34));
	}
	return l;
}

bool HelpScene::init()
{
    if ( !Layer::init() )
    {
        return false;
    }
	touched = false;
	Size visibleSize = Director::getInstance()->getVisibleSize();
	
	//setKeyboardEnabled(true);
	auto keyboard = EventListenerKeyboard::create();
	keyboard->onKeyReleased = CC_CALLBACK_2(HelpScene::onKeyReleased, this);
	getEventDispatcher()->addEventListenerWithSceneGraphPriority(keyboard, this);
	
	//setTouchEnabled(true);
	auto listener = EventListenerTouchOneByOne::create();
	listener->onTouchBegan = CC_CALLBACK_2(HelpScene::onTouchBegan, this);
	listener->onTouchMoved = CC_CALLBACK_2(HelpScene::onTouchMoved, this);
	listener->onTouchEnded = CC_CALLBACK_2(HelpScene::onTouchEnded, this);
	getEventDispatcher()->addEventListenerWithSceneGraphPriority(listener, this);

	addChild(BackgroundWidget::create("bg", Color3B(20, 20, 80)));

	addChild(Vignette::create());

	auto layer = Layer::create();
	layer->setTag(1);
	addChild(layer);

	auto bg = LayerColor::create(C4Fto4B(0.12f, 0.02f, 0.15f, 1.0f), visibleSize.width, visibleSize.height);
	bg->setPositionY(PY(0.82f));
    addChild(bg);
    
	auto hooker = TopAnchor::create();
    addChild(hooker, 1);
	auto pMenu = Menu::create();
	pMenu->setPosition(Point::ZERO);
	//hooker->addChild(bg);
    hooker->addChild(pMenu);

	auto txt1 = Label::createWithTTF(LS("HowToPlay"), NORMAL_TTF, 48.0f);
	txt1->setPosition(VCP(0.22f, 0.935f));
	txt1->setAnchorPoint(Point(0, 0.5f));
	txt1->setHorizontalAlignment(TextHAlignment::LEFT);
	txt1->setColor(Color3B::WHITE);
    
    hooker->addChild(txt1);
	//addChild(txt1, 5);

	auto btnBack = MAKEBUTTON("btnBackBlackUp", "btnBackBlackDn", HelpScene::btnBackPressed);
	pMenu->addChild(btnBack, 2);
	btnBack->setPosition(VCP(0.03f, 0.935f));
	btnBack->setAnchorPoint(Point(0, 0.5f));

	auto playerLayer = makeItem("manRt1", LS("HelpPlayer"), '@');
	playerLayer->setPosition(CP(0.2f, 0.80f));
	layer->addChild(playerLayer);

	auto robotLayer = makeItem("robot", LS("HelpRobot"), 'R');
	robotLayer->setPosition(CP(0.2f, 0.50f));
	robotLayer->SPRITEREF->runAction(
		RepeatForever::create(Sequence::createWithTwoActions(
		ScaleTo::create(1.0f, 1.0f), ScaleTo::create(1.0f, 0.9f)
		)));
	layer->addChild(robotLayer);

	auto holeLayer = makeItem("hazard", LS("HelpBlackhole"), '!');
	holeLayer->setPosition(CP(0.2f, 0.10f));
	holeLayer->SPRITEREF->runAction(RepeatForever::create(RotateBy::create(2.0f, 365.0f)));
	holeLayer->SPRITEREF->runAction(RepeatForever::create(Sequence::createWithTwoActions(
		TintTo::create(0.5f, 210, 255, 255), TintTo::create(0.5f, 255, 0, 0)
		)));
	layer->addChild(holeLayer);

	auto starLayer = makeItem("dronesGoal", LS("HelpStar"), 'X');
	starLayer->setPosition(CP(0.2f, -0.10f));
	starLayer->SPRITEREF->runAction(RepeatForever::create(Sequence::createWithTwoActions(
		TintTo::create(1.0f, 255, 255, 255), TintTo::create(1.0f, 255, 255, 0)
		)));
	starLayer->SPRITEREF->runAction(RepeatForever::create(Sequence::createWithTwoActions(
		DelayTime::create(10.0f), RotateBy::create(1.0, -360)
		)));
	layer->addChild(starLayer);

	auto gemLayer = makeItem("gem", LS("HelpGem"), 'g');
	gemLayer->setPosition(CP(0.2f, -0.30f));
	layer->addChild(gemLayer);

	auto creditsY = -0.20f;

	auto creditsLayer3 = makeText(LS("HelpConclusion"), 38.0f);
	creditsLayer3->setPosition(CP(0.5f, creditsY-0.3f));
	creditsLayer3->setHorizontalAlignment(TextHAlignment::CENTER);
	creditsLayer3->setAnchorPoint(Point(0.5f, 1.0f));
	layer->addChild(creditsLayer3);
	
	auto facebook = MAKESPRITE("facebook");
	facebook->setPosition(CP(0.5f, creditsY - 0.6f));
	layer->addChild(facebook);
	return true;
}

static const float bounds = 0;
bool HelpScene::onTouchBegan(Touch* t, Event*)
{
    getChildByTag(1)->stopAllActions();
	if (!touched)
	{
		touched = true;
		touchId = t->getID();
		
		initY = getChildByTag(1)->getPositionY();
		
		lastY = t->getLocation().y;
		touchY = t->getLocation().y;
		schedule(schedule_selector(HelpScene::updateLastY), 0.2f);
		return true;
	}
	return false;
}


void HelpScene::onTouchMoved(Touch* t, Event*)
{
	if (touched && t->getID() == touchId)
	{
		Point position = t->getLocationInView();
		position = Director::getInstance()->convertToGL(position);
		Size winSize = Director::getInstance()->getWinSize();
		float newY = (t->getLocation() - t->getStartLocation()).y + initY;
		if (newY < 0) newY = 0;
		if (newY > (PY(bounds)+PY(1.0f))) newY = (PY(bounds)+PY(1.0f));
		getChildByTag(1)->setPositionY(newY);
		touchY = t->getLocation().y;
	}
}

void HelpScene::onTouchEnded(Touch* t, Event*)
{
	unschedule(schedule_selector(HelpScene::updateLastY));
	touched = false;
    float targetY = getChildByTag(1)->getPositionY() + 2.0f * (t->getLocation().y - lastY);
    if (targetY < 0) targetY = 0;
    if (targetY > (PY(bounds)+PY(1.0f))) targetY = (PY(bounds)+PY(1.0f));
    if (targetY != getChildByTag(1)->getPositionY())
    {
        getChildByTag(1)->runAction(
			EaseIn::create(MoveTo::create(1.0f, Point(0, targetY)), 0.2f));
    }

	log(nToString(targetY / PY(1.0f)));
	//btnBackPressed(this);
}

void HelpScene::updateLastY(float)
{
	lastY = touchY;
}

void HelpScene::btnBackPressed(Ref*)
{
	Director::getInstance()->popScene(); //replaceScene(TransitionFadeDown::create(1.0f, MainMenu::scene()));
}

void HelpScene::onKeyReleased(EventKeyboard::KeyCode keyCode, Event*)
{
	if (keyCode == EventKeyboard::KeyCode::KEY_ESCAPE)
		btnBackPressed(this);
}