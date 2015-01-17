#include "helpers.h"
#include "BackgroundWidget.h"
#include "BundleStoreScene.h"
#include "BundleMainMenuScene.h"
#include "Widgets.h"
#include "Ooze.h"

Scene* BundleStore::createSceneWithText(string text)
{
    Scene* scene = Scene::create();
    auto hooker = TopAnchor::create();
    scene->addChild(hooker);
    auto pMenu = Menu::create();
	pMenu->setPosition(Point::ZERO);
	hooker->addChild(pMenu, 1);
    
    auto btnMenu = MAKEBUTTON("btnBackUp", "btnBackDn", [](Ref*){
        Director::getInstance()->popScene();
    });
    btnMenu->setPosition(VCP(-1, 1));
    btnMenu->runAction(
                       Sequence::createWithTwoActions(DelayTime::create(0.5f), MoveTo::create(0.5f, VCP(0,1))));
	
	btnMenu->setAnchorPoint(Point(-0.5, 1.5));
	btnMenu->setScale(1.3f);
	pMenu->addChild(btnMenu);
    
    auto label = Label::createWithSystemFont(text, "Arial", 72.0f);
    label->setPosition(CP(0.5f, 0.5f));
    label->setColor(Color3B::WHITE);
    scene->addChild(label);
    
    return scene;
}

Scene* BundleStore::createPreStoreScene()
{
#if (CC_TARGET_PLATFORM != CC_PLATFORM_IOS)
	return createStoreScene();
#endif

    if (Purchase::AvailablePurchaseItems.size() > 0)
    {
        return createStoreScene();
    }
    
    Purchase::FetchItems(false);
    return createSceneWithText("Loading Store...");
}

Scene* BundleStore::createStoreScene()
{
    Ooze::InstantSend({{"Event", "CreateStoreScene"}});
    Scene *scene = Scene::create();
	BundleStore *layer = BundleStore::create();
    scene->addChild(layer);
    return scene;
}

Scene* BundleStore::createBuyingScene()
{
    return createSceneWithText("Buying item...");
}

Scene* BundleStore::createBuyPassedScene()
{
    Ooze::InstantSend({{"Event", "CreateBuyPassedScene"}});
#if IS_IOS
    ShowPopup("Store", "Item successfully purchased!");
#endif
    return createStoreScene();
}

Scene* BundleStore::createBuyFailedScene()
{
#if IS_IOS
    ShowPopup("Store", "Item purchase failed.");
#endif
    return createStoreScene();
}

Scene* BundleStore::createRestorePassedScene(bool success)
{
#if IS_IOS
    if (success)
        ShowPopup("Store", "Items restored.");
    else
        ShowPopup("Store", "Restore failed.");
#endif
    return createStoreScene();
}

Scene* BundleStore::createRestoringScene()
{
    return createSceneWithText("Connecting...");
}

Node* BundleStore::createBackground()
{
	Size visibleSize = Director::getInstance()->getVisibleSize();

    Node* result = Node::create();

#if NEAT_BUNDLE
	Color4B bgcolor_color = Color4B::WHITE;
	auto bgcolor = LayerColor::create(bgcolor_color, visibleSize.width, visibleSize.height);
	bgcolor->setPosition(Point::ZERO);
	bgcolor->setTag(2000);
	result->addChild(bgcolor);
    
	result->addChild(BackgroundWidget::createWithoutBackground());
#else
	result->addChild(BackgroundWidget::create("bg", Color3B(20, 20, 100)));
#endif

	auto vig = Sprite::create("vig.png");
	auto vigSize = vig->getContentSize();
	vig->setScaleX(visibleSize.width / vigSize.width);
	vig->setScaleY(visibleSize.height / vigSize.height);
	vig->setPosition(CP(0.5f, 0.5f));
	result->addChild(vig);
    
    return result;
}

bool BundleStore::init()
{
    addChild(createBackground());
    
    auto hooker = TopAnchor::create();
    hooker->setTag(HOOKER_TAG);
    addChild(hooker, 1);
	auto pMenu = Menu::create(); 
	pMenu->setPosition(Point::ZERO);
	hooker->addChild(pMenu);

	float fontSize = 42.0f;

    auto gems = GemsLayer::createInside();
    gems->setPosition(VCP(0.97f, 0.97f));
    gems->setAnchorPoint(Point(1,1));
    gems->setTag(GEMS_TAG);
    hooker->addChild(gems);
    
	auto btnMenu = MAKEBUTTON("btnBackUp", "btnBackDn", BundleStore::menuCloseCallback);
	btnMenu->setPosition(VCP(0.03f, 0.97f));
	btnMenu->setAnchorPoint(Point(0, 1.0f));
	btnMenu->setScale(1.3f);
	pMenu->addChild(btnMenu);
    
    auto menu2 = Menu::create();
    addChild(menu2);
    menu2->setPosition(Point::ZERO);
    
    auto SpriteSize = MAKESPRITE("btnUp")->getContentSize();
    SpriteSize.height *= 1.1f;
    
    //float btnY = 0.7f;
    int itemsCount = (int)Purchase::AvailablePurchaseItems.size();
    if (itemsCount > 1 && GameSettings::FastRechargeBought) itemsCount--;
    float btnY = PY(0.5f) + itemsCount * SpriteSize.height * 0.5f;
    btnY /= PY(1.0f);
    
    
    auto gamelogo = MAKESPRITE("storelogo");
	addChild(gamelogo);
    gamelogo->setAnchorPoint(Point(0.45f, 0.2f));
	gamelogo->setPosition(CP(0.5f, btnY));
	gamelogo->runAction(RepeatForever::create(Sequence::createWithTwoActions(
                        ScaleTo::create(3.0f, 0.9f),
                        ScaleTo::create(3.0f, 1.0f)
                        )));
    
    float i = 0;
    log(nToString((int)Purchase::AvailablePurchaseItems.size()));
#if IS_IOS
    for (auto item : Purchase::AvailablePurchaseItems)
    {
        string itemId = item->getItemId()->getCString();
        if (itemId == FAST_REGENERATE_PRODUCT_ID && GameSettings::FastRechargeBought) continue;
        auto btn = MakeButton("btnUp", "btnDn", item->getName()->getCString(), fontSize, CC_CALLBACK_1(BundleStore::btnBuyPressed, this), NORMAL_TTF);
        btn->setPosition(CP(0.5f, btnY -= 0.1f));
        btn->setTag(i++);
        buttonItems.push_back(item);
        menu2->addChild(btn);
    }
#endif
    if (!GameSettings::FastRechargeBought)
    {//\nSuper Unlock unlocks everything\nand gives you unlimited access.
        auto regenHelp = Label::createWithTTF("Super Generator gives you a free\nGem every 150 seconds.", NORMAL_TTF, 28.0f);
        regenHelp->setColor(Color3B::WHITE);
        regenHelp->setHorizontalAlignment(TextHAlignment::CENTER);
        regenHelp->setAnchorPoint(Point(0.5f, 1.0f));
        regenHelp->enableGlow(Color4B::BLACK);
        regenHelp->setPosition(CP(0.5f, btnY - 0.06f));
        addChild(regenHelp);
    }
    
    auto timer = GemTimerLayer::createInside();
    timer->setAnchorPoint(Point(0.0f, 0.0f));
    timer->setPosition(CP(0.05f, 0.01f));
    timer->setTag(TIMER_TAG);
    addChild(timer);
    
    auto up = MAKESPRITE("btnRestore");
    auto dn = MAKESPRITE("btnRestore");
	dn->setColor(Color3B(150, 150, 150));
	
	auto restore = MenuItemSprite::create(up, dn, CC_CALLBACK_1(BundleStore::btnRestorePressed, this));
    restore->setAnchorPoint(Point(1, 0));
    restore->setPosition(CP(0.95f, 0.01f));
    menu2->addChild(restore);
    
    scheduleUpdate();
    
	auto keyboard = EventListenerKeyboard::create();
	keyboard->onKeyReleased = CC_CALLBACK_2(BundleStore::onKeyReleased, this);
	getEventDispatcher()->addEventListenerWithSceneGraphPriority(keyboard, this);

    return true;
}

void BundleStore::update(float)
{
    if (lastGems != GameSettings::GetGems())
    {
        string text = nToString(GameSettings::GetGems()) + " Gem";
        if (GameSettings::GetGems() != 1) text += "s";
        static_cast<Label*>(getChildByTag(HOOKER_TAG)->getChildByTag(GEMS_TAG)->getChildByTag(0))->setString(text);
        lastGems = GameSettings::GetGems();
    }
    if (lastTime != Purchase::GetRegenTimeLeft())
    {
        static_cast<Label*>(getChildByTag(TIMER_TAG)->getChildByTag(0))->setString(Purchase::GetRegenTimeLeftText());
        lastTime = Purchase::GetRegenTimeLeft();
    }
}

void BundleStore::btnBuyPressed(Ref* r)
{
#if IS_IOS
    int tag = static_cast<Node*>(r)->getTag();
    Purchase::Buy(buttonItems[tag]);
#endif
    Director::getInstance()->replaceScene(TransitionFade::create(0.5f, createBuyingScene()));
}

void BundleStore::btnRestorePressed(Ref*)
{
#if IS_IOS
    Purchase::Restore();
#endif
    Director::getInstance()->replaceScene(TransitionFade::create(0.5f, createRestoringScene()));
}

void BundleStore::menuCloseCallback(Ref*)
{
	Director::getInstance()->popScene();
}

void BundleStore::onKeyReleased(EventKeyboard::KeyCode keyCode, Event* event)
{
	if (keyCode == EventKeyboard::KeyCode::KEY_ESCAPE)
		menuCloseCallback(this);
}