#include "ChooserScene.h"
#include "MainMenuScene.h"
#include "PreGameScene.h"
#include "helpers.h"
#include "DronesScene.h"
#include "EpisodeChooserScene.h"
#include "BackgroundWidget.h"
#include "Widgets.h"

Scene* ChooserScene::scene(int offset)
{
    Scene *scene = Scene::create();
	ChooserScene *layer = ChooserScene::create(offset);
    scene->addChild(layer);
    return scene;
}

ChooserScene* ChooserScene::create(int offset)
{
	ChooserScene *pRet = new ChooserScene();
	if (pRet && pRet->init(offset))
	{
		pRet->autorelease();
		return pRet;
	}
	else
	{
		delete pRet;
		pRet = NULL;
		return NULL;
	}
}

static const int lCount = 3;
static const int rCount = 4;
static int offset;

// on "init" you need to initialize your instance
bool ChooserScene::init(int offset)
{
	::offset = offset;
    //////////////////////////////
    // 1. super init first
    if ( !Layer::init() )
    {
        return false;
    }

	Size visibleSize = Director::getInstance()->getVisibleSize();

#if !defined(BURGERMAN)
	addChild(BackgroundWidget::create("bg" + nToString(GameSettings::CurrentEpisode),
                                      Color3B(200, 200, 200)));
#else
	addChild(BackgroundWidget::createWithDefaultBackground());
#endif

	addChild(Vignette::create());

	string title = LS(EpisodeInfos[GameSettings::CurrentEpisode].title);

    auto hooker = TopAnchor::create();
    addChild(hooker, 1);

	auto episodetitle = Label::createWithTTF(title, FANCY_TTF, 60.0f);
	episodetitle->setHorizontalAlignment(TextHAlignment::CENTER);
	episodetitle->setPosition(VCP(0.5f, 0.9f));
	hooker->addChild(episodetitle);

	auto pMenu = Menu::create();
	pMenu->setPosition(Point::ZERO);
	hooker->addChild(pMenu, 1);

#if (CC_TARGET_PLATFORM != CC_PLATFORM_ANDROID)
	auto btnMenu = MAKEBUTTON("btnMenuUp", "btnMenuDn", ChooserScene::btnMenuPressed);
	btnMenu->setPosition(VCP(0, 1));
	btnMenu->setAnchorPoint(Point(-0.5, 1.5));
	btnMenu->setScale(1.3f);
	pMenu->addChild(btnMenu);
#endif
    
	auto pLevels = Menu::create();
	pLevels->setPosition(Point::ZERO);
	pLevels->setTag(0);
	hooker->addChild(pLevels);

	const float startX = 0.2f;
	const float startY = 0.25f;

	auto bSprite = Sprite::createWithSpriteFrameName("circle");
	auto contentsize = bSprite->getContentSize();
	float off = 1.0f - (2.0f * startX);
	off /= (float) (lCount - 1);
	float offy = 1.0f - (startY * 2.0f);
	offy /= (float) (rCount - 1);
    log("Unlocked Level ID:" + nToString(GameSettings::GetUnlockedLevelId()));
	float y = startY;

	GLubyte cR = 255;
	for (int r = 0; r < rCount; r++)
	{
		float x = startX;
		GLubyte cG = 200; // static_cast<GLubyte>(200.0f * (float) offset / (float) GameSettings::MaximumLevelId);
		GLubyte cB = 20; // +200 - static_cast<GLubyte>(200.0f * (float) offset / (float) GameSettings::MaximumLevelId);;
		for (int i = 0; i < lCount; i++)
		{
			int levelId = offset + (rCount - (r + 1)) * lCount + i;
			if (levelId >= GameSettings::GetMaximumLevelId()) break;

            Node* btn;
			if (levelId <= GameSettings::GetUnlockedLevelId())
			{
				btn = MAKEBUTTON("circle", "circleDn", ChooserScene::btnLevelPressed);
				btn->setTag(100 + levelId);
				pLevels->addChild(btn);
				blobs.push_back(btn);
				btn->setPosition(VCP(x, y));

				Label* lbl = Label::createWithBMFont(NORMAL_FNT, nToString(levelId + 1).c_str());
				lbl->setColor(Color3B::WHITE);
				lbl->setPosition(VCP(x, y));
                numbers.push_back(lbl);
				this->addChild(lbl, 1);

				btn->setColor(Color3B(cR * 0.5, cG * 0.5, cB * 0.5));
			}
			else
			{
				btn = Sprite::createWithSpriteFrameName("circleLk");
				btn->setTag(100 + levelId);
				hooker->addChild(btn);
				blobs.push_back(btn);
				btn->setPosition(VCP(x, y));
			}
			btn->setScale(0.8f);
#if ENABLE_IAD
            //btn->setScale(0.8f);
#endif
			cG *= 0.5;
			cB *= 1.7;
			cB = cB > 255 ? 255 : cB;

			x += off;
		}

		cR *= 0.7;
		y += offy;
	}

	if (offset > 0)
	{
		auto backBtn = MAKEBUTTON("btnBackUp", "btnBackDn", btnBackPressed);
		backBtn->setPosition(CP(0.1f, 0.1f));
		backBtn->setScale(1.3f);
		pMenu->addChild(backBtn);
	}

	if (offset + lCount * rCount < GameSettings::GetMaximumLevelId())
	{
		auto nextBtn = MAKEBUTTON("btnNextUp", "btnNextDn", btnNextPressed);
		nextBtn->setPosition(CP(0.9f, 0.1f));
		nextBtn->setScale(1.3f);
		pMenu->addChild(nextBtn);
	}

	schedule(schedule_selector(ChooserScene::bloop), 2.0f, kRepeatForever, 4.0f);
    
    
	auto keyboard = EventListenerKeyboard::create();
	keyboard->onKeyReleased = CC_CALLBACK_2(ChooserScene::onKeyReleased, this);
	getEventDispatcher()->addEventListenerWithSceneGraphPriority(keyboard, this);
    return true;
}

void ChooserScene::btnBackPressed(Ref*)
{
	PlayEffect("page_turn_4.wav");
	Director::getInstance()->replaceScene(
		TransitionFlipX::create(0.6f, ChooserScene::scene(offset - rCount*lCount)));
}

void ChooserScene::btnNextPressed(Ref*)
{
	PlayEffect("page_turn_4.wav");
	Director::getInstance()->replaceScene(
		TransitionFlipX::create(0.6f, ChooserScene::scene(offset + rCount*lCount)));
}

void ChooserScene::btnMenuPressed(Ref*)
{
	PlayEffect("bottle_pop_2.wav");
	Director::getInstance()->replaceScene(
		TransitionFade::create(0.5f, EpisodeChooser::createScene()));
}

void ChooserScene::bloop(float)
{
	if (!blobs.size()) return;

	Node* blob = CHOOSE(blobs);
	blob->stopAllActions();
	blob->runAction(Sequence::createWithTwoActions(
		ScaleTo::create(0.2f, 0.9f), ScaleTo::create(0.2f, 0.8f)
		));
}

void ChooserScene::btnLevelPressed(Ref* pSender)
{
	PlayEffect("bottle_pop_2.wav");
	GameSettings::DronesLastScore = 0;
	GameSettings::LastMoves = 0;
	GameSettings::LastTime = 60;
	GameSettings::LastKills = 0;
	GameSettings::LastWins = 0;
	GameSettings::CurrentLevelId = static_cast<Node*>(pSender)->getTag() - 100;
	Director::getInstance()->replaceScene(TransitionFade::create(0.75f, Drones::DronesScene::scene()));
}

void ChooserScene::onKeyReleased(EventKeyboard::KeyCode keyCode, Event*)
{
	if (keyCode == EventKeyboard::KeyCode::KEY_ESCAPE)
		btnMenuPressed(nullptr);
}