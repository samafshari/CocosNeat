#ifndef __PLATFORMERLAYER_H__
#define __PLATFORMERLAYER_H__

#include <string>
#include <vector>
#include <map>
#include <set>
#include <stack>

#include "cocos2d.h"
#include "helpers.h"
#include "PlatformerEntity.h"

#include "Box2D/Box2D.h"
USING_NS_CC;

//number of blocks visible in the screen
namespace Platformer
{
	const int SCREENXCOUNT = (20);
	const float BLOCK_SIZE = (DESIGN_X / SCREENXCOUNT);
	const int SCREENYCOUNT = (DESIGN_Y / BLOCK_SIZE);
	const int THRESHOLD_X = 2;
	const int THRESHOLD_Y = 1;

	extern Layer* inventoryLayer;
	class PlatformerScene;

	enum class GameStates
	{
		Initializing,
		Ready,
		Paused,
		Cutscene,
		Done,
		Editor
	};

	class PlatformerLayer : public Layer, public b2ContactListener
	{
	public:
		virtual bool init() override;

		//Block size is 32px on SD resolution (480x320)
		//Ignoring one row for dashboard, we'll have a
		//15(X) by 9(Y) board.
		//On 960.0x resolution, block size is 64x64.

		static const float blockSize;

		static PlatformerLayer* create(PlatformerScene*, vector<string>);

		bool didWin;
		bool forceNoScroll;
		bool doFlipX, doFlipY, doTranspose;
		float initGravityScale;
		PlatformerScene* scene;
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

		void clearMap();
		void parseLevel(vector<string> levelData);
		string dumpLevel();

		Entity* ParseAddEntity(char charcode, Position p);
		void initEntity(Entity*);
		void initAllEntities();
		Entity* makeEntity(char charcode);
		Point positionToPoint(Position);
		void addEntity(Entity* e, Position pos, int zorder = 0);
		void removeEntity(Entity* e);
		void switchToReady(float = 0);
		void move(Point dir);
		bool move(Entity*, Directions, Position* dest = nullptr, bool simulation = false, float animDuration = 0.1f); //Moves the object @pos to direction d.
		void teleport(Entity*, Point);
		void broadcast(Entity*, const string&);
		void playerDied();

		void pickEggplant();
		void gameOver(bool win = true);
		void doGameOver(float = 0);
		void startCutscene(Ref* sender = nullptr);

		void spawnPlayer(Position);

		void kill(Entity* e);
		void flipGravity(bool);
		bool gravityFlipped();

		string message;
		b2World* world;

		struct TeleportQueueData
		{
			Entity* entity;
			Point target;
		};
		vector<TeleportQueueData> teleportQueue;

		void update(float);
		void updatePhysics(float);
		virtual void BeginContact(b2Contact* contact) override;
		virtual void EndContact(b2Contact* contact) override;

		~PlatformerLayer();
	};
}
#endif