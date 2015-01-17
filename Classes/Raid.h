#ifndef __RAIDSCENE_H__
#define __RAIDSCENE_H__

#include "helpers.h"

namespace Raid
{
	const float PLAYER_SPEEDY = 100.0f;
	const float POWER_COEF = 0.5f;
	const float PLAYER_SPEED = 30.0f;
	const float PLAYER_DAMP = 0.99f;
	const float PLAYER_STOP_THRES = 0.5f;
	const float PLAYER_MAX_SPEED = 32.0f;
	const float SPEED_BOOST_EASY = 40.0f;
	const float SPEED_BOOST_MEDIUM = 50.0f;
	const float SPEED_BOOST_HARD = 55.0f;
	const float SPEED_BOOST_BRUTAL = 60.0f;
	const float FUEL_USE = 0.02f; //percent per second
	const float FUEL_GAIN = 0.5f; //percent per second
	const int SCORE_TO_LIFE = 750;

	const int TAG_RAIDLAYER = 1000;
	const int kFUEL_DASHBOARD = 10001;
	const int kFUEL_GAUGE = 10002;
	const int kTXT_SCORE = 10003;
	const int kLYR_LIVES = 10004;
	const int kLYR_SCORE = 10005;
	const int kLYR_LEVEL = 10006;
	const int kTXT_LEVEL = 10007;
	const int kLVL_PARTICLE = 10008;
	const int kSTEAM_PARTICLE = 10009;
	const int kPLAYER_EXPLOSION = 10010;
	const int kBAR_SCORE = 10011;
	const int kLYR_SPEEDBAR = 10012;
	const int kLYR_SPEEDGUAGE = 10013;
	const int kFLAME_PARTICLE = 10014;
	const int kLYR_LIFEBAR = 10015;
	const int kLYR_LIFEGUAGE = 10016;
	const int kLBL_SPEED = 10017;
	const int kLBL_LIFE = 10018;
	const int kBTN_LASER = 10019;
	const int kBTN_ROCKET = 10020;
	const int kBTN_GUN = 10021;
	const int kBTN_NORMALLASER = 100210;
	const int kLYR_MENU = 10022;
	const int kTXT_SPEED = 10023;
	const int kIMG_REDVIG = 10024;
	const int kLYR_CANVAS = 10025;
	const int kTXT_FLASHTEXT = 10026;
	const int kTAG_PARTICLE = 100001;

	enum class Directions
	{
		Left = -1,
		Right = 1,
		Stop = 0
	};

	enum class GameStates
	{
		Intro = 0,
		Playing,
		Dying
	};

	enum class Items
	{
		Fuel = 0,
		Ship,
		Heli,
		House,
		Tree,
		Bridge,
		Jet
	};

	class Entity : public Sprite
	{
	public:
		static const float SCALE;

        static ParticleSystemQuad* MakeExplosion();
		static Entity* create(string frameName);
		static Entity* MakeEntity(Items itype, Point position, bool flip);

		bool init(string frameName);
		void moveX(float offset);
		void moveY(float offset);
		void die();
		void performCollect(float);
		void stopParticles(float);

		bool noclip;
		float vel;
		float wakeUpDistance;
		int scorePoints;
		bool immortal;
		bool dead;
		bool collect;
		bool ghost;
		bool touched;

		Items itemType;
	};

	class Rocket : public Node
	{
	public:
		float speed;
		vector<Node*> projectilesToDelete;

		CREATE_FUNC(Rocket);
		Rocket();
		bool init();
		virtual void hit(Node* projectile, bool wall);
		virtual void update(float) override;
		bool removeInvisibleProjectiles();
		bool moveWithPlayer;
	};

	class Missile : public Rocket
	{
	public:
		CREATE_FUNC(Missile);
	};

	class Laser : public Rocket
	{
	public:
		CREATE_FUNC(Laser);
		bool init();
		void hit(Node*, bool) override;
	};

	class Gun : public Rocket
	{
	public:
		CREATE_FUNC(Gun);
		bool init();
	};

	class NormalLaser : public Rocket
	{
	public:
		CREATE_FUNC(NormalLaser);
		bool init();
	};

	class RaidLayer : public cocos2d::Layer
	{
	private:
		float _innerWallTop;
		float _outerWallTop;
		//bool _rocketLaunched;
		float _accel;
		float _boost;

	public:
		float oldViewHeight;
		void viewHeightChanged();
		static RaidLayer* CurrentGame;

		Layer* levels;
		Layer* canvas;
		Layer* gameLayer;

		Entity* player;
		Node* rockets;
		GameStates state;
		Menu* menu;
		Size dashboardSize;

		int score, scoreInLife;
		float fuel;
		float lives;
		float _entityBoost;
		bool scoreActive;
		bool _refueling;
		int selectedWeapon;

		CREATE_FUNC(RaidLayer);
		virtual bool init();

		void createMenu();
		void switchWeaponPressed(Ref*);

		void createGame();
		void reloadGame();
		void loadLevel(int);
		void loadLevel(vector<string> data, Color4B color);

		void move(float angle, float radius = 1.0f);
		void teleport(float x);
		void shoot();
		void setBoost(float);

		void updateGame(float = 0);
		void updatePlayerMovement(float);
		void updateRocketMovement(float);
		void stopRocket();
		void updatePhysics(float);
		void createRocket();
		void playerDie();
		void advanceLevel();
		void giveFuel(float);
		void spendFuel(float);
		void giveLife();
		void giveScore(int);
		void updateLivesUI();
		void updateFuelUI();
		void updateSpeedUI();
		void updateScoreTo1UpUI();

		void switchToPlaying(float = 0);
		void switchToTimer(float = 0);
		void switchToLevelTxt(float = 0);
		void switchToScoreTxt(float = 0);

		void createBridgeExplosion(Point);
		void stopBridgeExplosion(float = 0);
		void removeBridgeExplosion(float = 0);
		void removeInvisibleEntities();
		void streamLevels();

		void createPlayerExplosion();
		void removePlayerExplosion(float = 0);
		void playFuelSound(float = 0);
		void gameOver();

		Label* winStatsLayer;
		void flashText(string text);
	};

	struct LevelEntityData
	{
		Items itype;
		Point position;
		bool flip;
	};

	class LevelGenerator
	{
	protected:
		static vector<Point> GenerateSection(int width, int height, float& yOffset);
		static vector<Point> GenerateTransition(int beginW, int endW, float& yOffset);
		static vector<Point> GenerateInnerWalls(vector<Point> inner);
		static vector<Point> GenerateInnerSection(vector<Point> outer, int width, int height, float& yOffset);
		static vector<Point> GenerateInnerTransition(vector<Point> outer, int beginW, int endW, float& yOffset);
		static vector<Point> SimplifyWalls(vector<Point> walls);

		static vector<LevelEntityData> GenerateOuterTreesAndHouses(vector<Point> outer, bool left);
		static vector<LevelEntityData> GenerateInnerTreesAndHouses(vector<Point> inner);
		static vector<LevelEntityData> GenerateEntities(vector<Point> outer, vector<Point> inner, int level);

		static float GetOuterWidth(vector<Point> outer, float y, int delta = 4);
		static float GetMinWidth(vector<Point> walls, float yBegin, float yEnd);
		static float GetMaxWidth(vector<Point> walls, float yBegin, float yEnd);
	public:
		static const float RIBBON_WIDTH;
		static const float RIBBON_HEIGHT;
		static const int MAX_SECTION_OUTER_RIBBONS;
		static const int MIN_SECTION_OUTER_RIBBONS;
		static const int MIN_SECTIONS;
		static const int MAX_SECTIONS;
		static const int MAX_OUTER_BLOCKS;
		static const int MIN_OUTER_BLOCKS;
		static const int BRIDGE_WIDTH_BLOCKS;
		static const int BRIDGE_RIBBONS;

		static const int ISLAND_THRESHOLD;
		static const int MIN_ISLAND_GAP;
		static const int MIN_ISLAND_LENGTH;

		static const Size HOUSE_SIZE;
		static const Size TREE_SIZE;
		static const int TREEHOUSE_CHANCE;
		static const float TREEHOUSE_DISTANCE_MIN;
		static const float TREEHOUSE_DISTANCE_MAX;
		static const Size ENTITY_SIZE;

		static vector<string> GenerateLevel(int levelId);
		static vector<string> ToString(vector<Point> outerWalls, vector<Point> innerWalls,
			vector<LevelEntityData> entities);
	};

	class RaidScene : public Layer
	{
	public:
		static int Level;

		RaidLayer* game;

		void onAcceleration(Acceleration* accel, Event*) override;

		static cocos2d::Scene* scene();
		virtual bool init();

		CREATE_FUNC(RaidScene);

		void btnBackPressed(Ref*);

		void onKeyReleased(EventKeyboard::KeyCode keyCode, Event* event) override;

		bool onTouchBegan(Touch*, Event*) override;
		void onTouchEnded(Touch*, Event*) override;
		void shoot(float);
		bool move(Touch*);
		void move(float angle, float radius = 1.0f);
		void boost(float tilt);
		void teleport(float x);
		void shoot();
        MenuItemSprite *btnSound;
        void btnSoundPressed(Ref*);
		void gameOver(bool backPressed=false);

	};
};
#endif
