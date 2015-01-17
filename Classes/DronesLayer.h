#ifndef __DRONESLAYER_H
#define __DRONESLAYER_H

#include <string>
#include <vector>
#include <map>
#include <set>
#include <stack>

#include "helpers.h"
#include "DronesEntity.h"

USING_NS_CC;


namespace Drones
{
	//number of blocks visible in the screen
	const float BLOCK_SCALE = (1.0f);
	const float SCREENXCOUNT = (DESIGN_X / (BLOCK_SCALE * 64.0f));
	const float SCREENYCOUNT = (DESIGN_Y / (BLOCK_SCALE * 64.0f) - 1.0f);
	const int THRESHOLD_X = 2;
	const int THRESHOLD_Y = 1;
	const float HOLDMOVEPERIOD = 0.7f;
	const float INVENTORYX_SHIFT =	0.03f;

	extern Layer* inventoryLayer;
	class DronesScene;

	enum class GameStates
	{
		Initializing,
		Ready,
		Paused,
		Cutscene,
		Done
	};

	class DronesLayer : public Layer
	{
	public:
		virtual bool init() override;

		//Block size is 32px on SD resolution (480x320)
		//Ignoring one row for dashboard, we'll have a
		//15(X) by 9(Y) board.
		//On 960.0x resolution, block size is 64x64.

		static const float blockSize;

		static DronesLayer* create(DronesScene*, vector<string>);

		bool didWin;
		bool forceNoScroll;
		bool doFlipX, doFlipY, doTranspose;
		DronesScene* scene;
		GameStates state;
		Layer* overlay;
		Player* player;
		Size gameSize;
		vector<Entity*> entities;
		vector<Entity*> getEntities(Position);
		int moves;
		int monsters;
		int kills;
		int eggplants;
		int lollis;
		int collectedGems;
		void clearMap();
		virtual void parseLevel(vector<string> levelData);

		void initEntity(Entity*);
		void initAllEntities();
		Entity* makeEntity(char charcode);
		Point positionToPoint(Position);
		void addEntity(Entity* e, Position pos, int zorder = 0);
		void removeEntity(Entity* e);
		void switchToReady(float = 0);
		bool move(Entity*, Directions, Position* dest = nullptr, bool simulation = false, float animDuration = 0.1f); //Moves the object @pos to direction d.
		void teleport(Entity*, Position);
		void broadcast(Entity*, const string&);
		void playerDied();
		void pickEggplant();
		void pickLolli();
		void lolliDied();

		void gameOver(bool win = true);
		void doGameOver(float = 0);
		void startCutscene(Ref* sender = nullptr);
		Position findSafePosition();
		void spawnBonus(float);
		void spawnPlayer(Position);

		void kill(Entity* e);
		string message;
	};
}
#endif