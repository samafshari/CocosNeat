#ifndef __BlobLayer_H__
#define __BlobLayer_H__

#include "helpers.h"
#include "Box2D/Box2D.h"

namespace Blob
{
	class Entity;

	enum class GameStates
	{
		Idle,
		Playing,
		Paused
	};

	class ScoreText : public Node
	{
	public:
		CREATE_FUNC(ScoreText);
		static ScoreText* create(int score, Point pos, Color3B col);

		void initWithScore(int score, Point pos, Color3B col);
		void collect(float = 0);
	};

	class BlobLayer : public Layer, public b2ContactListener
	{
	public:
		map<int, Sprite*> ring;
		Label* bigText;
		int lastObservedScore;

		b2Vec2 ambientForce;
		float regenPeriod;
		float minRegenPeriod;
		float regenTime;
		b2Vec2 forceFieldPos;

		virtual bool init();
		CREATE_FUNC(BlobLayer);
		
		GameStates state;
		b2World* world;
		set<Entity*> entities;
		void update(float);
		void generate(float=0);
		void updatePhysics(float);
		void giveScore(int, Entity* = nullptr);
		void gameOver();

		virtual void BeginContact(b2Contact* contact) override;
		virtual void EndContact(b2Contact* contact) override;

		bool onTouchBegan(Touch*, Event*) override;
		void onTouchMoved(Touch*, Event*) override;
		void onTouchEnded(Touch*, Event*) override;

		void onAcceleration(Acceleration* accel, Event*);

		void checkCombo(float);
		void flashBigText(string, Color3B);

		~BlobLayer();
	};
}

#endif