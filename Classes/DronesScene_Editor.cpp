#include "DronesScene.h"
#include "MapGenerator.h"
#include <iostream>
#include <fstream>

using namespace Drones;

void DronesScene::editorInit()
{
	editorChosenItem = '.';

	editorItemsMenu = Menu::create();
	addChild(editorItemsMenu);

	auto touch = EventListenerTouchOneByOne::create();
	touch->onTouchBegan = CC_CALLBACK_2(DronesScene::editorTouchBegan, this);
	getEventDispatcher()->addEventListenerWithSceneGraphPriority(touch, this);

	auto keyboard = EventListenerKeyboard::create();
	keyboard->onKeyPressed = CC_CALLBACK_2(DronesScene::editorKeyboardPressed, this);
	getEventDispatcher()->addEventListenerWithSceneGraphPriority(keyboard, this);
}

bool DronesScene::editorTouchBegan(Touch*, Event*)
{
	return false;
}

void DronesScene::editorKeyboardPressed(EventKeyboard::KeyCode k, Event*)
{
	if (k == EventKeyboard::KeyCode::KEY_N) editorGenerateRandomMap();
	if (k == EventKeyboard::KeyCode::KEY_U) editorUndo();
	if (k == EventKeyboard::KeyCode::KEY_I) editorRedo();
	if (k == EventKeyboard::KeyCode::KEY_Y)
	{
		editorSaveMap();
		editorGenerateRandomMap();
	}
}

void DronesScene::editorGenerateRandomMap(MapGenerator::MapMeta meta)
{
	//generate random map
	auto gen = MapGenerator::Generate(meta);
	if (GameSettings::LastWins % 2 == 0 && gameType == GameTypes::Random)
	{
		//auto pos = MapGenerator::AddItem('g', gen, 4, meta.width, meta.height);
		//log(nToString(pos.x) + "," + nToString(pos.y));
		//log(nToString(gen[pos]));
	}
	else if (gameType == GameTypes::Eggplant)
	{
		int robotCount = min(5, GameSettings::LastWins);
		for (; robotCount >= 0; robotCount--) 
			MapGenerator::AddItem('m', gen, 3, meta.width, meta.height);
	}
	auto level = MapGenerator::Flatten(gen);
	
	level.push_back("noscroll");
	level.push_back("scale 0.7");
	editorLoadMap(level);
}

void DronesScene::editorUndo()
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

void DronesScene::editorRedo()
{
	if (editorRedoList.empty())
	{
		log("Redo stack empty.");
		return;
	}

	editorLoadMap(editorRedoList.top());
	editorRedoList.pop();
}

void DronesScene::editorLoadMap(vector<string> level, bool addToUndoStack)
{
	if (addToUndoStack && !levelData.empty())
		editorUndoList.push(levelData);
	levelData = level;

	auto gameParent = game->getParent();
	gameParent->removeChild(game);
	game = DronesLayer::create(this, levelData);
	gameParent->addChild(game);
}

void DronesScene::editorSaveMap()
{
#ifdef WIN32
	ofstream myfile;
	myfile.open ("maps/random/" + nToString((int)time(0)) + ".tly");
	for (auto line : levelData)
		myfile << line << endl;
	myfile.close();
#else
	//OOZE
#endif
}