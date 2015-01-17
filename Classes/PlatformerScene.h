#ifndef __GAMESCENE_H
#define __GAMESCENE_H

#include "helpers.h"
#include "PlatformerLayer.h"
#include "InputController.h"
#include "Sidebar.h"
#include <stack>

namespace Platformer
{
	const float BTNDELTA = 0.16f;
	const int GAMEPARENTTAG = 5000;
	const int OLDSTATSTAG = 5001;
	const int NEWSTATSTAG = 5002;

	const float buttonScale = 1.2f;

	enum class GameTypes
	{
		Level
	};
	class PlatformerScene : public Layer
	{
	protected:
		bool over;
		MenuItemSprite *btnSound;
		Layer* inventoryLayer;
		GameTypes gameType;
		VBar* vbar;

	public:
		vector<string> levelData;
		vector<Directions> moveHistory;

		virtual bool init();
		void addInput();
		void addGame();
		void addBackground(string filename, bool anim, Color3B tint);
		void createNormalGame();

		static Scene* createScene();
		static Scene* createScene(vector<string> levelData);

		CREATE_FUNC(PlatformerScene);
		PlatformerLayer* game;
		Node* gameParent;
		Node* bgParent;

		timeval moveBeginTime;

		void btnBackPressed(Ref*);
		void btnSoundPressed(Ref*);
		void advanceLevel(float = 0);

		void alignCamera(float=0);
		void moveCamera(float lX, float lY, float duration = 0);// 0.2f);

		void onKeyReleased(EventKeyboard::KeyCode keyCode, Event* event) override;
		bool move(Point dir, bool doAlignCamera = true);
		bool jump(Touch*, Event*);
		void cleanUpGame();

		std::vector<InputController*> inputs;

		bool dead;
		bool scrollable;
		bool hintVisible;

		//editor code
		char editorChosenItem;
		Menu* editorItemsMenu;
		stack<vector<string>> editorUndoList;
		stack<vector<string>> editorRedoList;

	private:
		MenuItemSprite *btnPlay, *btnPeek, *btnItem, *btnGod;
		bool peekEnabled;
	public:
		void editorInit();
		bool editorTouchBegan(Touch*, Event*);
		void editorTouchMoved(Touch*, Event*);
		void editorTouchEnded(Touch*, Event*);

		bool _shift, _ctrl, _alt;
		void editorKeyboardPressed(Key_t, Event*);
		void editorKeyboardReleased(Key_t, Event*);
		void editorGenerateRandomMap();
		void editorUndo();
		void editorRedo();
		void editorLoadMap(vector<string>, bool addToUndoStack = true);
		void editorSaveMap();
		void editorPickChar(char);
		void editorPasteItem(Point p);
		void editorBtnPlayPressed(Ref*);
		void editorBtnItemsPressed(Ref*);
		void editorBtnGodPressed(Ref*);
		void editorBtnSavePressed(Ref*);
		void editorBtnOpenPressed(Ref*);
		void editorBtnSavePressed_callback(string filename);
		void editorBtnOpenPressed_callback(string filename);
		void editorBtnPeekPressed(Ref*);
	};
}
#endif // __GAMESCENE_H
