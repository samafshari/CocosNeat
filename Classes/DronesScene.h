#ifndef __DRONESSCENE_H
#define __DRONESSCENE_H

#include "helpers.h"
#include "DronesLayer.h"
#include "InputController.h"
#include "MapGenerator.h"
#include "Sidebar.h"
#include <stack>

namespace Drones
{
	const int TAG_WIN = 667;
	const float buttonScale = 1.2f;

	const float BTNDELTA = 0.16f;
	const int GAMEPARENTTAG = 5000;
	const int OLDSTATSTAG = 5001;
	const int NEWSTATSTAG = 5002;

	enum class GameTypes
	{
		Level,
		Random,
		Eggplant
	};

	class DronesScene : public Layer
	{
	protected:
		bool over;
		MenuItemSprite *btnSound, *btnController;
		Layer* inventoryLayer;
		float menuY;
		GameTypes gameType;

	public:
		vector<string> levelData;
		vector<Directions> moveHistory;

		virtual bool init();
		void addInput();
		void addGame();
		void addBackground();
		void createNormalGame();
		void createArcadeGame();

		static Scene* StartRandomGame();
		static Scene* StartEggplantGame();

		static Scene* scene();
		static Scene* randomScene();
		static Scene* eggplantScene();

		CREATE_FUNC(DronesScene);
		DronesLayer* game;
		HBar* pMenu;

		Node* gameParent;
		Node* bgParent;

		timeval moveBeginTime;

		void btnBackPressed(Ref*);
		void btnRefreshPressed(Ref*);
		void btnSoundPressed(Ref*);
		void btnHelpPressed(Ref*);
		void btnNextLevel(Ref*);
		void btnPrevLevel(Ref*);
		void btnChooseLevel(Ref*);
		void advanceLevel(float = 0);
		void btnSkipLevel(Ref*);
		void btnSurrenderPressed(Ref*);

		void alignCamera();
		void moveCamera(float lX, float lY, float duration = 0.1f);

		void onKeyReleased(EventKeyboard::KeyCode keyCode, Event* event) override;
		bool move(Directions dir, bool doAlignCamera = true);
		void cleanUpGame();

		Layer* createGameStats(Layer* parent, float width, float height, Color3B foreColor = Color3B::BLACK, Color4B backColor = Color4B::WHITE);
		void updateGameStats(float = 0);
		void saveGameStats();

		Layer* createRandomStats(Layer* parent, float width, float height, Color3B foreColor = Color3B::BLACK, Color4B blackColor = Color4B::WHITE);
		void updateRandomStats(float = 0);
		void updateRandomTimer(float = 0);
		void randomGameOver(bool backPressed = false);

		std::vector<InputController*> inputs;

		bool dead;
		bool scrollable;
		bool hintVisible;
		bool moved;

		//editor code
		char editorChosenItem;
		Menu* editorItemsMenu;
		stack<vector<string>> editorUndoList;
		stack<vector<string>> editorRedoList;

		void editorInit();
		bool editorTouchBegan(Touch*, Event*);
		void editorKeyboardPressed(EventKeyboard::KeyCode, Event* event);
		void editorGenerateRandomMap(MapGenerator::MapMeta = GAME_OF_DRONES_MAPMETA);
		void editorUndo();
		void editorRedo();
		void editorLoadMap(vector<string>, bool addToUndoStack = true);
		void editorSaveMap();
	};
}
#endif
