#include "Widgets.h"
#include "BundleStoreScene.h"
#include "ShaderLayer.h"

bool GemsLayer::init()
{
    if ( !Node::init() ) return false;
 
	return true;

    menu = Menu::create();
    menu->setPosition(Point::ZERO);
    addChild(menu);
#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)
	auto ins = createInside();
    addChild(ins);
	setContentSize(ins->getContentSize());
#else
    putStuffInMenu();
#endif
    scheduleUpdate();
    return true;
}

Sprite* GemsLayer::createInside()
{
	auto gemIcon = MAKESPRITE("gemholder");
    gemIcon->setPosition(Point::ZERO);
    gemIcon->setAnchorPoint(Point::ZERO);
    
	string text = nToString(GameSettings::Gems) + " Gem";
	if (GameSettings::Gems != 1) text += "s";
    
	auto gemIconSize = gemIcon->getContentSize();
    
	auto label = Label::createWithTTF(text, NORMAL_TTF, 30.0f);
    label->setAnchorPoint(Point(1.0f, 0.5f));
	label->setPositionX(gemIconSize.width * 0.85f);
	label->setColor(Color3B::BLACK);
    label->setPositionY(gemIconSize.height * 0.5f);
    label->setHorizontalAlignment(TextHAlignment::LEFT);
    label->setTag(0);
	gemIcon->addChild(label);
    
    return gemIcon;
}

void GemsLayer::putStuffInMenu()
{
    lastGems = GameSettings::Gems;
    
    menu->removeAllChildren();
    
    auto iconUp = createInside();
    auto iconDn = createInside();
    iconDn->setColor(Color3B(150, 150, 150));
    
    auto btn = MenuItemSprite::create(iconUp, iconDn, CC_CALLBACK_1(GemsLayer::pressed, this));
    btn->setAnchorPoint(Point::ZERO);
    btn->setPosition(Point::ZERO);
    menu->addChild(btn);
    
    btn->runAction(Sequence::createWithTwoActions(ScaleTo::create(0.2f, 1.1f), ScaleTo::create(0.1f, 1.0f)));
    setContentSize(iconUp->getContentSize());
}

void GemsLayer::update(float)
{
    if (GameSettings::Gems != lastGems)
    {
#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)
		removeAllChildren();
		addChild(createInside());
#else
        putStuffInMenu();
#endif
		lastGems = GameSettings::Gems;
    }
}

void GemsLayer::pressed(Ref*)
{
#if (CC_TARGET_PLATFORM != CC_PLATFORM_ANDROID)
    Director::getInstance()->pushScene(TransitionFade::create(1.0f, BundleStore::createPreStoreScene()));
#endif
}

/////////// PAUSELAYER
void PauseLayer::Pause(Node* p, function<void()> _resumefunc, function<void()> _stopfunc)
{
    Scene* scene = Scene::create();
    p->retain();
    p->removeFromParentAndCleanup(false);

	auto shader = ShaderLayer::create("blur.glsl", "blur.vsh");
	scene->addChild(shader);
	
    shader->addChild(p);
    
    PauseLayer* layer = PauseLayer::create();
    scene->addChild(layer);
    layer->parentScene = p;
	layer->resumefunc = _resumefunc;
	layer->stopfunc = _stopfunc;
    layer->postInit();
    
    recursivePause(p);
    Director::getInstance()->replaceScene(scene);
    log("Paused");
}

bool PauseLayer::init()
{
    if (!Layer::init()) return false;
    
    
    return true;
}

void PauseLayer::postInit()
{
    addChild(LayerColor::create(Color4B(0, 0, 0, 100), DESIGN_X, DESIGN_Y));
    addChild(Vignette::create());
    
    auto leftbar = LayerColor::create(Color4B::WHITE, PX(0.1f), PY(0.18f));
    leftbar->ignoreAnchorPointForPosition(false);
    leftbar->setAnchorPoint(Point(1.25f, -0.1f));
    leftbar->setPosition(CP(0.5f, 0.5f));
    addChild(leftbar);
    
    auto rightbar = LayerColor::create(Color4B::WHITE, PX(0.1f), PY(0.18f));
    rightbar->ignoreAnchorPointForPosition(false);
    rightbar->setAnchorPoint(Point(-0.25f, -0.1f));
    rightbar->setPosition(CP(0.5f, 0.5f));
    addChild(rightbar);
    
    auto label = Label::createWithTTF(LS("Paused"), NORMAL_TTF, 64.0f);
    label->setAnchorPoint(Point(0.5f, 1.5f));
    label->setPosition(CP(0.5f, 0.5f));
    addChild(label);
    
    auto touchListener = EventListenerTouchOneByOne::create();
	touchListener->setSwallowTouches(true);
    touchListener->onTouchBegan = CC_CALLBACK_2(PauseLayer::touched, this);
	touchListener->onTouchEnded = CC_CALLBACK_2(PauseLayer::touchEnded, this);
    getEventDispatcher()->addEventListenerWithSceneGraphPriority(touchListener, this);

	//auto ribbon = LayerColor::create(Color4B(40, 40, 40, 255), DESIGN_X, PY(0.1f));
	//ribbon->ignoreAnchorPointForPosition(false);
	//ribbon->setAnchorPoint(Point(0.5f, 0.5f));
	//ribbon->setPosition(CP(0.5f, 0.3f));
	//addChild(ribbon);

	Menu* menu = Menu::create();
	addChild(menu);
	menu->setPosition(Point::ZERO);

	auto soundButton = SoundButton::create();
	soundButton->setPosition(CP(0.8f, 0.3f));
	menu->addChild(soundButton);

	auto backButton = MAKEBUTTON("Platformer_playup", "Platformer_playdn", btnBackPressed);
	backButton->setPosition(CP(0.2f, 0.3f));
	menu->addChild(backButton);

	auto exitButton = MAKEBUTTON("Platformer_stopup", "Platformer_stopdn", btnExitPressed);
	exitButton->setPosition(CP(0.5f, 0.3f));
	menu->addChild(exitButton);

}

void PauseLayer::btnBackPressed(Ref*)
{
	touchEnded(nullptr, nullptr);
}

void PauseLayer::btnExitPressed(Ref*)
{
	if (stopfunc)
	{
		btnBackPressed(nullptr);
		stopfunc();
		return;
	}

	Director::getInstance()->popScene();
}

bool PauseLayer::touched(Touch*, Event*)
{
    return true;
}

void PauseLayer::touchEnded(Touch*, Event*)
{
	Scene* scene = Scene::create();
	parentScene->removeFromParentAndCleanup(false);
	scene->addChild(parentScene);
	recursiveResume(parentScene);
	Director::getInstance()->replaceScene(scene);
	if (resumefunc) resumefunc();
}

void PauseLayer::onKeyReleased(EventKeyboard::KeyCode keyCode, Event* event)
{
	if (keyCode == EventKeyboard::KeyCode::KEY_ESCAPE)
	{
		btnBackPressed(nullptr);
	}
}

////////// GEMTIMERLAYER
bool GemTimerLayer::init()
{
    if ( !Node::init() ) return false;
    if (Purchase::RegenLocked) return true;
	return true;

    auto menu = Menu::create();
    menu->setPosition(Point::ZERO);
    addChild(menu);
    
    up = createInside();
    dn = createInside();
    dn->setColor(Color3B(150, 150, 150));
    
    auto btn = MenuItemSprite::create(up, dn, CC_CALLBACK_1(GemTimerLayer::pressed, this));
    btn->setAnchorPoint(Point::ZERO);
    btn->setPosition(Point::ZERO);
    menu->addChild(btn);
    
    setContentSize(up->getContentSize());
    lastTime = Purchase::GetRegenTimeLeft();
    schedule(schedule_selector(GemTimerLayer::updateText));
    return true;
}

Sprite* GemTimerLayer::createInside()
{
    string spriteName = "gemholdertimer";
	if (GameSettings::FastRechargeBought) spriteName = "gemholdertimersuper";
    auto gemIcon = MAKESPRITE(spriteName);
    //gemIcon->setFlippedX(true);
    gemIcon->setPosition(Point::ZERO);
    gemIcon->setAnchorPoint(Point::ZERO);
    
	auto gemIconSize = gemIcon->getContentSize();
    
	auto label = //Label::createWithTTF(Purchase::GetRegenTimeLeftText(), FANCY_TTF, 40.0f);
		Label::createWithTTF(Purchase::GetRegenTimeLeftText(), NORMAL_TTF, 30.0f);
    label->setAnchorPoint(Point(0.0f, 0.5f));
	label->setPositionX(gemIconSize.width * 0.15f);
    label->setPositionY(gemIconSize.height * 0.5f);
	label->setColor(Color3B::BLACK);
    label->setHorizontalAlignment(TextHAlignment::RIGHT);
	gemIcon->addChild(label);
    label->setTag(0);

    return gemIcon;
}

void GemTimerLayer::pressed(Ref*)
{
#if (CC_TARGET_PLATFORM != CC_PLATFORM_ANDROID)
    Director::getInstance()->pushScene(TransitionFade::create(1.0f, BundleStore::createPreStoreScene()));
#endif
}

void GemTimerLayer::updateText(float)
{
    if (lastTime == Purchase::GetRegenTimeLeft()) return;
    static_cast<Label*>(up->getChildByTag(0))->setString(Purchase::GetRegenTimeLeftText());
    static_cast<Label*>(dn->getChildByTag(0))->setString(Purchase::GetRegenTimeLeftText());
}

FacebookShareButton* FacebookShareButton::create(bool highscore, int score, Games game)
{
	auto e = new FacebookShareButton();
	e->isHighscore = highscore;
	e->score = score;
	e->game = game;
	if (e && e->init())
	{
		e->autorelease();
	}
	else
	{
		delete e;
		e = NULL;
	}
	return e;
}

Sprite* FacebookShareButton::createInside(bool highscore, int score)
{
	//string spriteName = highscore ? "facebooksharehighscore" : "facebookshare";
	string spriteName = "facebookshare";
	auto icon = MAKESPRITE(spriteName);

	icon->setPosition(Point::ZERO);
	icon->setAnchorPoint(Point::ZERO);

	return icon;
}

bool FacebookShareButton::init()
{
	if (!Node::init()) return false;

	auto menu = Menu::create();
	menu->setPosition(Point::ZERO);
	addChild(menu);

	auto up = createInside(isHighscore, score);
	auto dn = createInside(isHighscore, score);
	dn->setColor(Color3B(150, 150, 150));

	auto btn = MenuItemSprite::create(up, dn, CC_CALLBACK_1(FacebookShareButton::pressed, this));
	btn->setAnchorPoint(Point::ZERO);
	btn->setPosition(Point::ZERO);
	menu->addChild(btn);

	setContentSize(up->getContentSize());
	return true;
}

void FacebookShareButton::pressed(Ref*)
{
    //intentional bug: player will be given a gem
    //regardless of whether they actually share or not.
    //if (isHighscore) GameSettings::GiveGems(2);
	//log("FacebookShareButton::pressed(highscore=" + nToString(isHighscore) + ")");

	ShareScore(score, game);
}

Vignette* Vignette::create()
{
	auto l = new Vignette();
	if (l && l->init())
	{
		l->autorelease();
	}
	else
	{
		delete l;
		return nullptr;
	}

	auto vig = Sprite::create("vig.png");
	auto vigSize = vig->getContentSize();
	vig->setScaleX(DESIGN_X / vigSize.width);
	vig->setScaleY(DESIGN_Y / vigSize.height);
	vig->setPosition(CP(0.5f, 0.5f));
	l->addChild(vig);

	return l;
}

SoundButton* SoundButton::create()
{
	SoundButton *ret = new SoundButton();
	string sprite = GameSettings::MuteSounds ? "btnMute" : "btnSound";
	string psprite = sprite + "Dn";
	sprite += "Up";
	ret->initWithNormalSprite(MAKESPRITE(sprite), MAKESPRITE(psprite), 
		Node::create(), CC_CALLBACK_1(SoundButton::btnSoundPressed, ret));
	ret->autorelease();
	return ret;
}

void SoundButton::btnSoundPressed(Ref*)
{
	GameSettings::MuteSounds = !GameSettings::MuteSounds;
	string sprite = GameSettings::MuteSounds ? "btnMute" : "btnSound";
	string psprite = sprite + "Dn";
	sprite += "Up";

	setSelectedImage(MAKESPRITE(psprite));
	setNormalImage(MAKESPRITE(sprite));

	PlayEffect("bottle_pop_2.wav");

	if (GameSettings::MuteSounds)
		SimpleAudioEngine::getInstance()->stopAllEffects();

	GameSettings::SaveSettings();
}

MusicButton* MusicButton::create()
{
	MusicButton *ret = new MusicButton();
	string sprite = GameSettings::IsMusicMuted ? "btnMusicOff" : "btnMusicOn";
	string psprite = sprite + "Dn";
	sprite += "Up";
	ret->initWithNormalSprite(MAKESPRITE(sprite), MAKESPRITE(psprite),
		Node::create(), CC_CALLBACK_1(MusicButton::btnMusicPressed, ret));
	ret->autorelease();
	return ret;
}

void MusicButton::btnMusicPressed(Ref*)
{
	GameSettings::SetMuteMusic(!GameSettings::IsMusicMuted);

	string sprite = GameSettings::IsMusicMuted ? "btnMusicOff" : "btnMusicOn";
	string psprite = sprite + "Dn";
	sprite += "Up";

	setSelectedImage(MAKESPRITE(psprite));
	setNormalImage(MAKESPRITE(sprite));

	GameSettings::SaveSettings();
}

Sprite* TiledBackground::create(string filename)
{
	Texture2D::TexParams texParams;
	texParams.magFilter = GL_LINEAR;
	texParams.minFilter = GL_LINEAR;
	texParams.wrapS = GL_REPEAT;
	texParams.wrapT = GL_REPEAT;
	Sprite* bgwrap = Sprite::create(filename, Rect(0, 0, DESIGN_X, DESIGN_Y));
	bgwrap->getTexture()->setTexParameters(texParams);
	bgwrap->setPosition(CP(0.5f, 0.5f));
	bgwrap->setOpacity(200);
	return bgwrap;
}

ModalDialog* ModalDialog::create(string text)
{
	auto l = new ModalDialog();
	l->btnOkPressed = nullptr;

	if (l && l->init(text))
	{
		l->autorelease();
	}
	else
	{
		delete l;
		return nullptr;
	}

	return l;
}

bool ModalDialog::init(string text)
{
	if (!Layer::init()) return false;

	addChild(LayerColor::create(Color4B(0, 0, 0, 20), DESIGN_X, DESIGN_Y));
	addChild(Vignette::create());

	auto listener = EventListenerTouchOneByOne::create();
	listener->onTouchBegan = CC_CALLBACK_2(ModalDialog::onTouchBegan, this);
	listener->setSwallowTouches(true);
	getEventDispatcher()->addEventListenerWithFixedPriority(listener, -127);

	return true;
}

bool ModalDialog::onTouchBegan(Touch* t, Event*)
{
	getEventDispatcher()->removeEventListener(listener);
	if (btnOkPressed != nullptr) btnOkPressed();
	removeFromParent();
	return false;
}