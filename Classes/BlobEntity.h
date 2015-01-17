#ifndef __BlobENTITY_H
#define __BlobENTITY_H
#include <string>
#include <iostream>
#include "helpers.h"
#include "Box2D/Box2D.h"

using namespace std;
USING_NS_CC;

namespace Blob
{
	class BlobLayer;
	
#define FIX_BASE ((void*)1)
#define FIX_AURA ((void*)2)

#define CIRCLE_SHAPE \
	virtual b2Fixture* createShape(b2FixtureDef* blockShapeDef) { return createCircleShape(blockShapeDef); }

	class Entity : public Node
	{
	protected:
		void explode(float) { timeBombExploded(); }
	public:
		set<b2Fixture*> _contacts;
		set<Entity*> neighbors;
		typedef Node Base;

		Entity();
		static Entity* MakeEntity(char charcode);
		static BlobLayer* Game;
		static vector<Color3B> Colors;
		
		Position position;
		bool dead;
		bool collect;
		bool bomb;
		char charcode;
		bool active;
		int color;
		bool controllable;
		int points;
		bool happy;

		set<Entity*> mates;

		Sprite* AddSprite(const vector<string>&);
		Sprite* AddSprite(const string&);

		virtual void explodeBomb();
		virtual void makeHappy();
		virtual void makeSad(float);
		virtual void disableControl(float);
		virtual void activateTimeBomb(float duration);
		virtual void touch(Entity* it, b2Fixture* myFixture, b2Fixture* itsFixture);
		virtual void touchEnded(Entity* it, b2Fixture* myFixture, b2Fixture* itsFixture);
		virtual void die();
		virtual void doCollect(float = 0);
		virtual b2Fixture* createShape(b2FixtureDef* blockShapeDef);
		void update(float);

		virtual void start();
		virtual void timeBombExploded() { die(); }
		virtual void fadeDie(float);
		Sprite* sprite, *eyel, *eyer, *mo, *alpha;
		Sprite* ring;
		b2Body* body;

		b2Fixture* createBoxShape(b2FixtureDef* blockShapeDef);
		b2Fixture* createCircleShape(b2FixtureDef* blockShapeDef);
		void syncBodyPosition();
	};

	class Blob : public Entity
	{
	public:
		typedef Entity Base;
		Blob();
		CREATE_FUNC(Blob);
		virtual bool init() override;
		virtual b2Fixture* createShape(b2FixtureDef* blockShapeDef) override;
		virtual void start() override;
	};
}
#endif