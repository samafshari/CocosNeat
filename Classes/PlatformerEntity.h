#ifndef __PLATFORMERENTITY_H
#define __PLATFORMERENTITY_H

#include <string>
#include <iostream>
#include "helpers.h"
#include "Box2D/Box2D.h"

namespace Platformer
{
	class PlatformerLayer;

	using namespace std;
	USING_NS_CC;

	enum class InventoryItems
	{
		Key1 = 20,
		Key2 = 21,
		Key3 = 22,
		Key4 = 23,
		Star = 30,
		Teleport1 = 40,
		Teleport2 = 41,
		Teleport3 = 42,
		Teleport4 = 43
	};

#define FIX_BASE ((void*)1)
#define FIX_FOOT ((void*)2)

#define CIRCLE_SHAPE \
	virtual b2Fixture* createShape(b2FixtureDef* blockShapeDef) { return createCircleShape(blockShapeDef); }

	class Entity : public Node
	{
	protected:
		void explode(float) { timeBombExploded(); }
		int _r, _g, _b;
		bool timeBombActive;
	public:
		int initZ;
		typedef Node Base;

		Entity();
		static Entity* MakeEntity(char charcode);
		static PlatformerLayer* Game;

		Position position;
		bool player;
		bool harmful;
		bool immortal;
		bool pushable;
		bool pusher;
		bool dead;
		bool collect;
		bool listener;
		char charcode;
		void sendMessage(const string&);
		Sprite* AddSprite(const vector<string>&);
		Sprite* AddSprite(const string&);

		virtual void activateTimeBomb(float duration);
		virtual bool potentialPassable(Entity*) { return dead; }
		virtual bool move(Directions);
		virtual void moved(Position oldPos) {}
		virtual bool touch(Entity* it, b2Fixture* myFixture, b2Fixture* itsFixture);
		virtual void touchEnded(Entity* it, b2Fixture* myFixture, b2Fixture* itsFixture) {}
		virtual bool touch(Entity*) { return false; }
		virtual void receiveMessage(Entity*, const string&) {}
		virtual void startGameCall() {}
		virtual void pauseCall() { unscheduleAllSelectors(); }
		virtual void setDefaultColor(int, int, int);
		virtual void shine(int, int, int, float timeIn = 0.1f, float timeOut = 0.5f);
		virtual void die();
		virtual void doCollect(float = 0);
		virtual void command(const string& line, int cpos = 0) {}
		virtual b2Fixture* createShape(b2FixtureDef* blockShapeDef);

		virtual void createPhysics();
		virtual void timeBombExploded() { die(); }
		virtual void fadeDie(float);
		virtual void gravityChanged(bool flipped);
		Sprite* sprite;
		b2Body* body;

		b2Fixture* createBoxShape(b2FixtureDef* blockShapeDef);
		b2Fixture* createCircleShape(b2FixtureDef* blockShapeDef);
		void syncBodyPosition();
		void moveTo(Point p, float t);
		void stopMoving();
		void moveAction(float = 0);
		Point moveDelta;
		Point moveDest;
		float moveTimeLeft;
		//Action* moveToAction; //reserved for movement command from gamelayer
	};

	class Fake : public Entity
	{
	public:
		typedef Entity Base;
		CREATE_FUNC(Fake);

		virtual bool potentialPassable(Entity*) override { return true; }
		virtual bool touch(Entity*) override { return true; }
		virtual void createPhysics();
	};

	class Wall : public Entity
	{
	private:
		int _playerContacts;
		bool _soundEnabled;
	public:
		bool tintOnTouch;
		typedef Entity Base;
		Wall();
		CREATE_FUNC(Wall);
		virtual bool potentialPassable(Entity* e) override { return false; }
		virtual bool touch(Entity*) override;

		virtual bool touch(Entity*, b2Fixture*, b2Fixture*) override;
		virtual void touchEnded(Entity*, b2Fixture*, b2Fixture*) override;
		void enableSound(float = 0);
	};

	class Box : public Wall
	{
	public:
		typedef Wall Base;
		Box();
		virtual void createPhysics() override;
		virtual bool touch(Entity*) override;
		CREATE_FUNC(Box);
	};

	class Interactive : public Entity
	{
	protected:
		int	_shineR, _shineG, _shineB;
		bool _labelPresent;
	public:
		bool tintOnTouch;
		char op;
		int num;
		Label* label;

		typedef Entity Base;
		CREATE_FUNC(Interactive);
		Interactive();
		virtual void startGameCall() override;

		Label* AddLabel(const string&);
		Label* AddLabel(void);
		virtual void updateLabel();

		virtual bool function() { return true; }
		virtual bool potentialPassable(Entity* e) override { return false; }
		virtual void command(const string&, int) override;
		void setShineColor(int r, int g, int b) {
			_shineR = r; _shineG = g; _shineB = b;
		}
		virtual bool touch(Entity* e) override {
			bool result = false;
			if (e->player) result = function();
			if (tintOnTouch) shine(_shineR, _shineG, _shineB);
			return false;
		}
	};

	class Operation : public Interactive
	{
	public:
		typedef Interactive Base;
		CREATE_FUNC(Operation);
		virtual bool init() override;
		virtual bool function() override;
	};

	class Once : public Operation
	{
	public:
		typedef Operation Base;
		Once();
		CREATE_FUNC(Once);
		virtual bool touch(Entity*) override;
	};

	class Condition : public Interactive
	{
	public:
		typedef Interactive Base;
		CREATE_FUNC(Condition);
		virtual bool potentialPassable(Entity*) override;
		virtual bool touch(Entity*) override;
	};

	class OnceCondition : public Condition
	{
	public:
		typedef Condition Base;
		CREATE_FUNC(OnceCondition);
		virtual bool touch(Entity*) override;
	};

	class EntityWithItem : public Entity
	{
	public:
		typedef Entity Base;
		InventoryItems item;
		CREATE_FUNC(EntityWithItem);

		virtual void command(const string&, int) override;
	};

	class Button : public EntityWithItem
	{
	public:
		typedef EntityWithItem Base;
		CREATE_FUNC(Button);

		virtual bool potentialPassable(Entity*) { return true; }
		virtual bool touch(Entity*) override;
	};

	class PressurePlate : public EntityWithItem
	{
	public:
		typedef EntityWithItem Base;
		CREATE_FUNC(PressurePlate);

		virtual bool potentialPassable(Entity*) { return true; }
		virtual bool touch(Entity*) override { return true; }
	};

	class Door : public EntityWithItem
	{
	private:
		bool pushed;
	public:
		typedef EntityWithItem Base;
		CREATE_FUNC(Door);
		Door();
		virtual bool potentialPassable(Entity* e) override { return false; }
		virtual bool touch(Entity*) override;
		virtual void receiveMessage(Entity*, const string&);
	};

	class Teleport : public EntityWithItem
	{
	public:
		typedef EntityWithItem Base;
		CREATE_FUNC(Teleport);
		virtual bool potentialPassable(Entity* e) override { return false; }
		virtual void createPhysics();
		//virtual bool touch(Entity*) override;
		void touchEnded(Entity* it, b2Fixture* myFixture, b2Fixture* itsFixture) override;
	};

	class TeleportDest : public EntityWithItem
	{
	public:
		typedef EntityWithItem Base;
		CREATE_FUNC(TeleportDest);
		virtual bool potentialPassable(Entity* e) override { return true; }
		virtual bool touch(Entity*) override { return true; }
		virtual void createPhysics();
	};

	class Player : public Entity
	{
	private:
		set<b2Fixture*> _floors;
		set<b2Fixture*> _contacts;
		bool _jumpFlag;
		float _angvel;
	public:
		typedef Entity Base;
		Player();
		CREATE_FUNC(Player);
		virtual bool init() override;
		virtual void pauseCall() override {};
		virtual void die() override;
		virtual bool touch(Entity*, b2Fixture*, b2Fixture*) override;
		virtual void touchEnded(Entity*, b2Fixture*, b2Fixture*) override;
		virtual b2Fixture* createShape(b2FixtureDef* blockShapeDef) override;
		virtual void createPhysics() override;
		bool canJump();
		void disableJump(float=0);
		void move(Point);
		void update(float);
		//CIRCLE_SHAPE;
	};

	class Water : public Entity
	{
		set<Entity*> _contacts;
	public:
		float power;

		typedef Entity Base;
		Water();
		CREATE_FUNC(Water);
		virtual bool init() override;
		virtual b2Fixture* createShape(b2FixtureDef* blockShapeDef) override;
		virtual bool touch(Entity*, b2Fixture*, b2Fixture*) override;
		virtual void touchEnded(Entity*, b2Fixture*, b2Fixture*) override;
		void update(float);
	};

	class Finish : public Entity
	{
	public:
		typedef Entity Base;
		CREATE_FUNC(Finish);
		virtual bool potentialPassable(Entity* e) override { return e->player; }
		virtual bool touch(Entity*) override;

		CIRCLE_SHAPE;
	};

	class Gem : public Entity
	{
	public:
		typedef Entity Base;
		CREATE_FUNC(Gem);
		virtual bool potentialPassable(Entity* e) override { return e->player; }
		virtual bool touch(Entity*) override;
	};

	class Eggplant : public Entity
	{
	public:
		typedef Entity Base;
		CREATE_FUNC(Eggplant);
		virtual b2Fixture* createShape(b2FixtureDef* blockShapeDef) override;
		virtual bool touch(Entity*) override;
	};

	class Glass : public Entity
	{
	public:
		typedef Entity Base;
		CREATE_FUNC(Glass);
		virtual bool potentialPassable(Entity* e) override { return e->player; }
		virtual bool touch(Entity*) override;
	};

	class Hazard : public Entity
	{
	public:
		Hazard();
		typedef Entity Base;
		CREATE_FUNC(Hazard);
		virtual bool potentialPassable(Entity* e) override { return true; }
		virtual bool touch(Entity*) override;

		CIRCLE_SHAPE;
	};

	class HPlatform : public Hazard
	{
	protected:
		float d;
		b2Vec2 v;
	public:
		HPlatform();
		typedef Hazard Base;
		CREATE_FUNC(HPlatform);
		virtual bool init() override;
		virtual b2Fixture* createShape(b2FixtureDef* blockShapeDef) override;
		virtual void createPhysics() override;
		virtual bool touch(Entity*) override;
		void update(float);
	};

	class VPlatform : public HPlatform
	{
	public:
		VPlatform();
		typedef HPlatform Base;
		CREATE_FUNC(VPlatform);
	};

	class Pit : public Entity
	{
	public:
		typedef Entity Base;
		CREATE_FUNC(Pit);
		virtual bool potentialPassable(Entity* e) override { return e->player; }
		virtual bool touch(Entity*) override;
	};

	class Monster : public Entity
	{
	private:
		float actTimer;
		int moveCount;
		int currentMoveCount;

	public:
		float interval;
		typedef Entity Base;
		Monster();
		CREATE_FUNC(Monster);
		virtual bool init();
		virtual void act(float);
		virtual void seek(Position, bool advanced = false);
		virtual bool touch(Entity*) override;
		float intelligence; //0.0f -> 1.0f
		virtual void die();

		CIRCLE_SHAPE;
	};

	class Robot : public Monster
	{
	public:
		typedef Monster Base;
		Robot();
		CREATE_FUNC(Robot);

		virtual void act(float) override;
		virtual void receiveMessage(Entity*, const string&) override;
	};
}
#endif