#include "PlatformerScene.h"
#include "PlatformerSwipeController.h"
#include "PlatformerTiltController.h"
#include "PlatformerKeyboardController.h"
#include "helpers.h"
#include "PlatformerEntity.h"
#include "Ooze.h"

namespace Platformer
{
	Scene* PlatformerScene::createScene()
	{
		return createScene(ReadLinesFromFile(GameSettings::GetLevelFilename()));
	}

	Scene* PlatformerScene::createScene(vector<string> level)
	{
		Scene *scene = Scene::create();

		Size visibleSize = Director::getInstance()->getVisibleSize();

		PlatformerScene *layer = PlatformerScene::create();
		layer->gameType = GameTypes::Level;
		layer->levelData = level;
		layer->addGame();
		layer->createNormalGame();
		scene->addChild(layer);

		return scene;
	}

	void PlatformerScene::addGame()
	{
		game = PlatformerLayer::create(this, levelData);
		gameParent->addChild(game);
		alignCamera();
		if (!game->forceNoScroll) schedule(schedule_selector(PlatformerScene::alignCamera));
	}

	void PlatformerScene::addBackground(string filename, bool anim, Color3B tint)
	{
		Size visibleSize = Director::getInstance()->getVisibleSize();

		Texture2D::TexParams texParams;
		texParams.magFilter = GL_LINEAR;
		texParams.minFilter = GL_LINEAR;
		texParams.wrapS = GL_REPEAT;
		texParams.wrapT = GL_REPEAT;
		Sprite* bgwrap = Sprite::create(filename, Rect(0, 0, visibleSize.width, visibleSize.height));
		bgwrap->getTexture()->setTexParameters(texParams);
		bgwrap->setPosition(CP(0.5f, 0.5f));
		bgwrap->setColor(tint);

		if (anim)
		{
			auto p = GLProgram::createWithFilenames("generic.vsh", "uv.glsl");
			bgwrap->setGLProgram(p);
		}

		bgParent->addChild(bgwrap);
	}

	bool PlatformerScene::init()
	{
		dead = false;
		over = false;
		scrollable = false;
		if (!Layer::init()) return false;

		bgParent = Node::create();
		bgParent->addChild(LayerColor::create(Color4B::BLACK, DESIGN_X, DESIGN_Y));

		addChild(bgParent);

		auto hooker = TopAnchor::create();
		addChild(hooker, 1);
		gameParent = Node::create();
		addChild(gameParent);
		gameParent->setTag(GAMEPARENTTAG);

		addInput();

		vbar = VBar::create();
		addChild(vbar, 1000);
		return true;
	}

	void PlatformerScene::createNormalGame()
	{
		Size visibleSize = Director::getInstance()->getVisibleSize();

		auto btnBack = MAKEBUTTON("btnExitUp", "btnExitDn", PlatformerScene::btnBackPressed);
		vbar->addChild(btnBack);

		string sprite = GameSettings::MuteSounds ? "btnMute" : "btnSound";
		string psprite = sprite + "Dn";
		//sprite += "Up";
		sprite = psprite;
		btnSound = MAKEBUTTON(sprite, psprite, btnSoundPressed);
		vbar->addChild(btnSound);

		editorInit();
	}

	bool PlatformerScene::jump(Touch*, Event*)
	{
		move(Point(0, 1));
		return false;
	}

	void PlatformerScene::addInput()
	{
		auto keyboard = EventListenerKeyboard::create();
		keyboard->onKeyReleased = CC_CALLBACK_2(PlatformerScene::onKeyReleased, this);
		getEventDispatcher()->addEventListenerWithSceneGraphPriority(keyboard, this);

		InputController* controller = SwipeController::create();
		controller->setTag(2001);

		inputs.push_back(KeyboardController::create());
		inputs.push_back(TiltController::create());
		//inputs.push_back(controller);

		for (auto input : inputs)
		{
			addChild(input, 10);
			input->reset();
		}

		auto listener = EventListenerTouchOneByOne::create();
		listener->onTouchBegan = CC_CALLBACK_2(PlatformerScene::jump, this);
		getEventDispatcher()->addEventListenerWithSceneGraphPriority(listener, this);

	}

	bool PlatformerScene::move(Point dir, bool doAlignCamera)
	{
		if (!game) return false;
		if (game->state != GameStates::Ready) return false;

		if (game->moves == 0)
		{
			if (gameType == GameTypes::Level)
			{
			}

			gettimeofday(&moveBeginTime, NULL);
		}

		game->move(dir);
		GameSettings::TotalMoves++;
		GameSettings::LastMoves++;

		if (doAlignCamera && !game->forceNoScroll) alignCamera();

		return true;
	}

	void PlatformerScene::btnBackPressed(Ref*)
	{
		Director::getInstance()->popScene();
	}

	void PlatformerScene::cleanUpGame()
	{
		for (auto input : inputs)
		{
			input->kill();
			input->unscheduleAllSelectors();
			input->unscheduleUpdate();
			removeChild(input);
		}
		game->state = GameStates::Initializing;
		game->unscheduleAllSelectors();
		Entity::Game = nullptr;
		dead = true;
	}

	void PlatformerScene::advanceLevel(float dt)
	{
		game->unscheduleAllSelectors();
		Entity::Game = nullptr;
		dead = true;
		for (auto input : inputs)
		{
			input->kill();
		}

		if (over && dt == 0) return;
		over = true;

		if (!game->didWin)
		{
			Director::getInstance()->replaceScene(PlatformerScene::createScene());
		}
		else
		{
			GameSettings::AdvanceLevel();
			Director::getInstance()->replaceScene(TransitionSlideInR::create(0.2f, PlatformerScene::createScene()));
		}
	}

	void PlatformerScene::onKeyReleased(EventKeyboard::KeyCode keyCode, Event* event)
	{
		if (keyCode == EventKeyboard::KeyCode::KEY_BACKSPACE)
			btnBackPressed(nullptr);
	}

	void PlatformerScene::alignCamera(float)
	{
		if (game->state == GameStates::Editor) return;
		float pX = game->player->getPositionX();
		float pY = game->player->getPositionY();

		float lX = DESIGN_X * 0.5f - pX;
		float lY = DESIGN_Y * 0.5f - pY;

		auto currentPos = game->getPosition();

		float width = DESIGN_X - vbar->getContentSize().width;
		float height = DESIGN_Y;

		if (lX > 0) lX = 0;
		if (lY > 0) lY = 0;
		if (lX < -(game->gameSize.width - width)) lX = -(game->gameSize.width - width);
		if (lY < -(game->gameSize.height - height)) lY = -(game->gameSize.height - height);

		auto oldlX = lX, oldlY = lY;
		if (game->gameSize.width < width || game->forceNoScroll)
			lX = (width - game->gameSize.width) * 0.5f;
		if (game->gameSize.height < height || game->forceNoScroll)
			lY = (height - game->gameSize.height) * 0.5f + game->blockSize * 0.5f;

		moveCamera(lX, lY);
	}

	static Action* _gamemoveToAction = nullptr;
	void PlatformerScene::moveCamera(float lX, float lY, float duration)
	{
		//if (/*(game->gameSize.width > DESIGN_X || game->gameSize.height > DESIGN_Y) &&*/ scrollable)
		//{
		//	if (duration > 0 && game->getPosition() != Point(lX, lY))
		//	{
		//		_gamemoveToAction = MoveTo::create(duration, Point(lX, lY));
		//		game->runAction(_gamemoveToAction);
		//	}
		//	else
		//	{
		//		game->setPosition(Point(lX, lY));
		//	}
		//}
		//else 
		game->setPosition(Point(lX, lY/* + game->blockSize * 0.5f*/));
	}

	void PlatformerScene::btnSoundPressed(Ref*)
	{
		GameSettings::MuteSounds = !GameSettings::MuteSounds;
		string sprite = GameSettings::MuteSounds ? "btnMute" : "btnSound";
		string psprite = sprite + "Dn";
		//sprite += "Up";
		sprite = psprite;
		btnSound->setSelectedImage(MAKESPRITE(psprite.c_str()));
		btnSound->setNormalImage(MAKESPRITE(sprite.c_str()));
		PlayEffect("bottle_pop_2.wav");
		GameSettings::SaveSettings();
	}
}