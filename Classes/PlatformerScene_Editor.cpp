#include "PlatformerScene.h"
#include "PlatformerMapGenerator.h"
#include "PlatformerLayer.h"
#include "FileScene.h"
#include "Ooze.h"

#include <iostream>
#include <fstream>

namespace Platformer
{
	void PlatformerScene::editorInit()
	{
		editorChosenItem = '8';
		peekEnabled = false;

		editorItemsMenu = Menu::create();
		addChild(editorItemsMenu);

		game->state = GameStates::Editor;

		auto touch = EventListenerTouchOneByOne::create();
		touch->onTouchBegan = CC_CALLBACK_2(PlatformerScene::editorTouchBegan, this);
		touch->onTouchMoved = CC_CALLBACK_2(PlatformerScene::editorTouchMoved, this);
		touch->onTouchEnded = CC_CALLBACK_2(PlatformerScene::editorTouchEnded, this);
		touch->onTouchCancelled = CC_CALLBACK_2(PlatformerScene::editorTouchEnded, this);
		getEventDispatcher()->addEventListenerWithSceneGraphPriority(touch, this);

		auto keyboard = EventListenerKeyboard::create();
		keyboard->onKeyPressed = CC_CALLBACK_2(PlatformerScene::editorKeyboardPressed, this);
		keyboard->onKeyReleased = CC_CALLBACK_2(PlatformerScene::editorKeyboardReleased, this);
		getEventDispatcher()->addEventListenerWithSceneGraphPriority(keyboard, this);
		_shift = _ctrl = _alt = false;

		vbar->addChild(btnItem = MAKEBUTTON("Platformer_itemsup", "Platformer_itemsdn", PlatformerScene::editorBtnItemsPressed));
		editorPickChar('8');

		vbar->addChild(MAKEBUTTON("Platformer_refup", "Platformer_refdn", PlatformerScene::editorBtnOpenPressed));
		vbar->addChild(MAKEBUTTON("Platformer_saveup", "Platformer_savedn", PlatformerScene::editorBtnSavePressed));

		{
			string sprite = game->state != GameStates::Editor ? "Platformer_play" : "Platformer_pause";
			string psprite = sprite + "dn";
			//sprite += "up";
			sprite = psprite;
			btnPlay = MAKEBUTTON(sprite, psprite, PlatformerScene::editorBtnPlayPressed);
			vbar->addChild(btnPlay);
		}
		
		{
			string sprite = peekEnabled ? "Platformer_move" : "Platformer_plus";
			string psprite = sprite + "dn";
			//sprite += "up";
			sprite = psprite;
			btnPeek = MAKEBUTTON(sprite, psprite, PlatformerScene::editorBtnPeekPressed);
			auto disabledImg = MAKESPRITE("Platformer_plusup");
			disabledImg->setOpacity(100);
			btnPeek->setDisabledImage(disabledImg);
			if (game->state != GameStates::Editor) btnPeek->setEnabled(false);
			vbar->addChild(btnPeek);
		}

		game->player->immortal = true;
		vbar->addChild(btnGod = MAKEBUTTON("Platformer_goddn", "Platformer_goddn", PlatformerScene::editorBtnGodPressed));
	}

	bool PlatformerScene::editorTouchBegan(Touch* touch, Event*)
	{
		if (game->state != GameStates::Editor) return false;

		if (peekEnabled)
		{
			return true; //enable grabbing move events
		}
		else
		{
			//paste mode
			editorPasteItem(touch->getLocation() - game->getPosition());
			return true;
		}

		return false;
	}

	void PlatformerScene::editorTouchMoved(Touch* touch, Event*)
	{
		if (peekEnabled)
		{
			//peek mode
			auto dv = touch->getLocation() - touch->getPreviousLocation() + game->getPosition();
			log("Current Cam: " + nToString(game->getPosition()) + " dv: " + nToString(dv));
			moveCamera(dv.x, dv.y);
		}
		else
		{
			//paste mode
			//editorPasteItem(game->convertToNodeSpace(touch->getLocation()));
		}
	}

	void PlatformerScene::editorTouchEnded(Touch* touch, Event*)
	{

	}

	void PlatformerScene::editorKeyboardPressed(Key_t k, Event*)
	{
		if (k == Key_t::KEY_SHIFT) _shift = true;
		if (k == Key_t::KEY_CTRL) _ctrl = true;
		if (k == Key_t::KEY_ALT) _alt = true;
		if (k == Key_t::KEY_F1) editorGenerateRandomMap();
		if (k == Key_t::KEY_F2) editorUndo();
		if (k == Key_t::KEY_F3) editorRedo();
		if (k == Key_t::KEY_F4)
		{
			editorSaveMap();
			editorGenerateRandomMap();
		}

		char c = getCharForKeyCode(k, _shift);
		if (c) editorPickChar(c);
	}

	void PlatformerScene::editorKeyboardReleased(Key_t k, Event*)
	{
		if (k == Key_t::KEY_SHIFT) _shift = false;
		if (k == Key_t::KEY_CTRL) _ctrl = false;
		if (k == Key_t::KEY_ALT) _alt = false;
	}

	void PlatformerScene::editorGenerateRandomMap()
	{
		//generate random map
		auto gen = MapGenerator::Generate(MAPMETA_Platformer);
		if (GameSettings::LastWins % 2 == 0)
		{
			auto pos = MapGenerator::AddItem('g', gen, 4);
			log(nToString(pos.x) + "," + nToString(pos.y));
			log(nToString(gen[pos]));
		}
		auto level = MapGenerator::Flatten(gen);

		level.push_back("noscroll");
		level.push_back("scale 0.7");
		editorLoadMap(level);
	}

	void PlatformerScene::editorUndo()
	{
		if (editorUndoList.empty())
		{
			log("Undo stack empty.");
			return;
		}

		editorRedoList.push(levelData);
		editorLoadMap(editorUndoList.top(), false);
		editorUndoList.pop();
	}

	void PlatformerScene::editorRedo()
	{
		if (editorRedoList.empty())
		{
			log("Redo stack empty.");
			return;
		}

		editorLoadMap(editorRedoList.top());
		editorRedoList.pop();
	}

	void PlatformerScene::editorLoadMap(vector<string> level, bool addToUndoStack)
	{
		Director::getInstance()->replaceScene(PlatformerScene::createScene(level));
	}

	void PlatformerScene::editorSaveMap()
	{
		//not implemented
	}

	void PlatformerScene::editorPickChar(char c)
	{
		auto e = Entity::MakeEntity(c);
		if (e && e->sprite)
		{
			auto s = Sprite::createWithSpriteFrame(e->sprite->getSpriteFrame());
			s->setColor(e->sprite->getColor());
			btnItem->setNormalImage(s);
			btnItem->setSelectedImage(Sprite::createWithSpriteFrame(e->sprite->getSpriteFrame()));
		}
		editorChosenItem = c;
	}

	void PlatformerScene::editorPasteItem(Point p)
	{
		p.x = (int) ((p.x) / game->blockSize) * game->blockSize;
		if (p.x > 0) p.x += game->blockSize;
		p.y = (int) ((p.y) / game->blockSize) * game->blockSize;
		if (p.y >= 0) p.y += game->blockSize;

		Entity* e = game->ParseAddEntity(editorChosenItem, Position{ -1, -1 });
		if (e)
		{
			e->setPosition(p);
			game->initEntity(e);
		}
		log("Pasted item " + nToString(editorChosenItem) + " @xy: " + nToString(p));
	}

	void PlatformerScene::editorBtnPlayPressed(Ref*)
	{
		if (game->state == GameStates::Editor)
		{
			game->state = GameStates::Ready;
			btnPeek->setEnabled(false);
		}
		else
		{
			game->state = GameStates::Editor;
			btnPeek->setEnabled(true);
		}

		string sprite = game->state != GameStates::Editor ? "Platformer_play" : "Platformer_pause";
		string psprite = sprite + "dn";
		//sprite += "up";
		sprite = psprite;
		btnPlay->setNormalImage(MAKESPRITE(sprite));
		btnPlay->setSelectedImage(MAKESPRITE(psprite));
	}

	void PlatformerScene::editorBtnItemsPressed(Ref*)
	{

	}

	void PlatformerScene::editorBtnGodPressed(Ref*)
	{
		game->player->immortal = !game->player->immortal;
		string sprite = "Platformer_god";
		if (game->player->immortal) sprite += "dn";
		else sprite += "up";
		btnGod->setNormalImage(MAKESPRITE(sprite));
	}

	void PlatformerScene::editorBtnSavePressed(Ref*)
	{
		FileScene::Caption = "Save level as";
		FileScene::Action = CC_CALLBACK_1(PlatformerScene::editorBtnSavePressed_callback, this);
		Director::getInstance()->pushScene(FileScene::createScene());
	}

	void PlatformerScene::editorBtnSavePressed_callback(string filename)
	{
		string dump = game->dumpLevel();
		GameSettings::Set("LevelData_" + filename, dump);
		log("Level data saved as " + filename);
		Ooze::InstantSend({{ "Event", "LevelSave" }, { "Data", dump }});
		GameSettings::SaveSettings();
	}

	void PlatformerScene::editorBtnOpenPressed(Ref*)
	{
		FileScene::Caption = "Load level from";
		FileScene::Action = CC_CALLBACK_1(PlatformerScene::editorBtnOpenPressed_callback, this);
		Director::getInstance()->pushScene(FileScene::createScene());
	}

	void PlatformerScene::editorBtnOpenPressed_callback(string filename)
	{
		string level = GameSettings::GetStr("LevelData_" + filename);
		if (level == "")
		{
			log("Level [" + filename + "] does not exist. Cannot load.");
			return;
		}
		editorLoadMap(ReadLines(level));
	}

	void PlatformerScene::editorBtnPeekPressed(Ref*)
	{
		peekEnabled = !peekEnabled;

		string sprite = peekEnabled ? "Platformer_move" : "Platformer_plus";
		string psprite = sprite + "dn";
		//sprite += "up";
		sprite = psprite;
		btnPeek->setNormalImage(MAKESPRITE(sprite));
		btnPeek->setSelectedImage(MAKESPRITE(psprite));
	}
}