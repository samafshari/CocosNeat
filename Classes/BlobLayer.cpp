#include "ExtraGameOverScene.h"
#include "BlobLayer.h"
#include "BlobScene.h"
#include "BlobEntity.h"
#include "Ooze.h"
#include "Blob.h"

namespace Blob
{
	ScoreText* ScoreText::create(int score, Point pos, Color3B col)
	{
		auto scoreText = ScoreText::create();
		scoreText->initWithScore(score, pos, col);
		return scoreText;
	}

	void ScoreText::initWithScore(int score, Point pos, Color3B col)
	{
		setPosition(pos);

		Label* label = Label::createWithBMFont(MATH_FNT, "+" + nToString(score));//, NORMAL_TTF, 46.0f);
		//label->setScale(1.1f);
		label->setHorizontalAlignment(TextHAlignment::CENTER);
		addChild(label);
		
		label->setOpacity(0);
		label->runAction(Sequence::create(
			FadeIn::create(0.3f),
			DelayTime::create(0.8f),
			FadeOut::create(0.3f),
			NULL));
/*
		col.r *= 0.5;
		col.g *= 0.5;
		col.b *= 0.5;*/

		label->setColor(Color3B::WHITE);
		label->disableEffect();

		label->runAction(TintTo::create(1.0f, col.r, col.g, col.b));
		label->runAction(MoveBy::create(1.0f, Point(0, PY(0.05f))));
		
		//label->enableGlow(Color4B(col));
		//label->enableShadow();

		scheduleOnce(schedule_selector(ScoreText::collect), 3.0f);
	}

	void ScoreText::collect(float)
	{
		removeAllChildren();
		removeFromParent();
	}

	/////////////////////////BLOB LAYER
	bool BlobLayer::init()
	{
		if (!Layer::init()) return false;
		Entity::Game = this;
		setContentSize(Size(DESIGN_X, DESIGN_Y));
		b2Vec2 gravity;
		gravity.Set(0.0f, -10.0f);
		world = new b2World(gravity);
		world->SetContinuousPhysics(true);
		world->SetContactListener(this);
		// Create edges around the entire screen
		// Define the ground body.
		b2BodyDef groundBodyDef;
		groundBodyDef.position.Set(0, 0); // bottom-left corner

		// Call the body factory which allocates memory for the ground body
		// from a pool and creates the ground box shape (also from a pool).
		// The body is also added to the world.
		auto _groundBody = world->CreateBody(&groundBodyDef);

		// Define the ground box shape.
		b2EdgeShape groundBox;
		// bottom
		groundBox.Set(b2Vec2(0, 0), b2Vec2(DESIGN_X / PTM_RATIO, 0));
		_groundBody->CreateFixture(&groundBox, 0);

		// top
		//groundBox.Set(b2Vec2(0, DESIGN_Y / PTM_RATIO), b2Vec2(DESIGN_X / PTM_RATIO, DESIGN_Y / PTM_RATIO));
		//_groundBody->CreateFixture(&groundBox, 0);

		// left
		groundBox.Set(b2Vec2(0, DESIGN_Y * 1.1f / PTM_RATIO), b2Vec2(0, 0));
		_groundBody->CreateFixture(&groundBox, 0);

		// right
		groundBox.Set(b2Vec2(DESIGN_X / PTM_RATIO, 0), b2Vec2(DESIGN_X / PTM_RATIO, DESIGN_Y * 1.1f / PTM_RATIO));
		_groundBody->CreateFixture(&groundBox, 0);

		state = GameStates::Playing;
		scheduleUpdate();
		//schedule(schedule_selector(BlobLayer::generate), 2.3f);

		bigText = Label::createWithTTF("", NORMAL_TTF, 200.0f);
		bigText->setHorizontalAlignment(TextHAlignment::CENTER);
		bigText->setVerticalAlignment(TextVAlignment::CENTER);
		bigText->setPosition(CP(0.5f, 0.45f));
		addChild(bigText, 10);

		auto touchListener = EventListenerTouchOneByOne::create();
		touchListener->onTouchBegan = CC_CALLBACK_2(BlobLayer::onTouchBegan, this);
		touchListener->onTouchMoved = CC_CALLBACK_2(BlobLayer::onTouchMoved, this);
		touchListener->onTouchEnded = CC_CALLBACK_2(BlobLayer::onTouchEnded, this);
		touchListener->onTouchCancelled = CC_CALLBACK_2(BlobLayer::onTouchEnded, this);
		getEventDispatcher()->addEventListenerWithSceneGraphPriority(touchListener, this);

		auto tilt = EventListenerAcceleration::create(CC_CALLBACK_2(BlobLayer::onAcceleration, this));
		getEventDispatcher()->addEventListenerWithSceneGraphPriority(tilt, this);

		ambientForce = b2Vec2_zero;
		regenPeriod = 1.5f;
		minRegenPeriod = regenPeriod * 0.5f;
		regenTime = 0;
		lastObservedScore = BlobScene::LastScore;
		schedule(schedule_selector(BlobLayer::checkCombo), 1.5f);

		/*
		auto ringParticle = ParticleGalaxy::create();
		ringParticle->setPositionType(ParticleSystem::PositionType::RELATIVE);
		ring->addChild(ringParticle);*/

		return true;
	}

	void BlobLayer::onAcceleration(Acceleration* accel, Event*)
	{
		if (state != GameStates::Playing) return;
		ambientForce = b2Vec2(accel->x, accel->y);
		//world->SetGravity(b2Vec2(accel->x * 30, -14));
	}

	void BlobLayer::generate(float)
	{
		auto e = Blob::create();
		addChild(e);
		entities.insert(e);
		e->setPosition(CP(
			//0.5f,			//
			RANDF2(0.2f,0.8f), 
			1.1f));
		e->start();
	}

	void BlobLayer::giveScore(int d, Entity* e)
	{
		if (state != GameStates::Playing) return;
		static_cast<BlobScene*>(getParent())->giveScore(d);
		if (e && !e->bomb)
		{
			addChild(ScoreText::create(d, e->getPosition(), Entity::Colors[e->color]));
		}
	}

	void BlobLayer::update(float dt)
	{
		if (state == GameStates::Playing)
		{
			updatePhysics(dt);
			regenTime += dt;
			if (regenTime > regenPeriod)
			{
				regenTime = 0;
				generate();
			}
			if (regenPeriod > minRegenPeriod) regenPeriod -= dt * 0.06f;
		}
	}
	
	void BlobLayer::updatePhysics(float dt)
	{
		const int velocityIterations = 1;
		const int positionIterations = 1;

		ambientForce.y -= dt * 6.0f;
		
		// Instruct the world to perform a single step of simulation. It is
		// generally best to keep the time step and iterations fixed.
		if (state == GameStates::Playing)
			world->Step(dt, velocityIterations, positionIterations);

		for (b2Body* b = world->GetBodyList(); b; b = b->GetNext())
		{
			if (b->GetUserData() != NULL) {
				//Synchronize the AtlasSprites position and rotation with the corresponding body
				Node* myActor = (Node*) b->GetUserData();
				auto aabb = myActor->getBoundingBox();
				myActor->setPosition(Point(
					b->GetPosition().x * PTM_RATIO,
					b->GetPosition().y * PTM_RATIO));
				myActor->setRotation(-1 * CC_RADIANS_TO_DEGREES(b->GetAngle()));

				Entity* e = dynamic_cast<Entity*>(myActor);
				if (e)
				{
					auto af = ambientForce;
					if (e->controllable) af *= 2.0f;
					b->ApplyForceToCenter(b2Vec2(af.x * 1400.0f * dt, af.y * dt), true);

					for (auto r : ring)
					{
						b2Vec2 rv = b2Vec2(r.second->getPositionX() / PTM_RATIO, r.second->getPositionY() / PTM_RATIO);
						const float maxPower = 1300.0f;
						b2Vec2 v = rv - b->GetPosition();
						float ir2 = 1.0f / v.LengthSquared();
						v.Normalize();
						float power = min(maxPower, maxPower * ir2);
						if (e->happy) power *= 1.2f;
						if (e->controllable) power *= 1.2f;
						v.x *= power;
						v.y *= power;
						b->ApplyForceToCenter(v, true);
					}

					if (e->getPositionY() > PY(1.0f) - getPositionY() && !e->controllable && !e->active)
					{
						gameOver();
					}
				}
			}
		}
	}
	
	void BlobLayer::gameOver()
	{
		state = GameStates::Idle;
		unscheduleAllSelectors();
		static_cast<BlobScene*>(getParent())->showGameOver();
	}

	void BlobLayer::BeginContact(b2Contact* contact)
	{
		if (state != GameStates::Playing) return;

		if (!contact->GetFixtureA() || !contact->GetFixtureB()) return;
		void* vb1 = contact->GetFixtureA()->GetBody()->GetUserData();
		void* vb2 = contact->GetFixtureB()->GetBody()->GetUserData();
		if (!vb1 || !vb2) return;

		Entity* e1 = static_cast<Entity*>(vb1);
		Entity* e2 = static_cast<Entity*>(vb2);

		if (e1->dead || e2->dead)
		{
			return;
		}
		e1->touch(e2, contact->GetFixtureA(), contact->GetFixtureB());
		e2->touch(e1, contact->GetFixtureB(), contact->GetFixtureA());

		b2ContactListener::BeginContact(contact);
	}
	
	void BlobLayer::EndContact(b2Contact* contact)
	{
		if (state != GameStates::Playing) return;

		b2ContactListener::EndContact(contact);

		if (!contact->GetFixtureA() || !contact->GetFixtureB()) return;
		void* vb1 = contact->GetFixtureA()->GetBody()->GetUserData();
		void* vb2 = contact->GetFixtureB()->GetBody()->GetUserData();
		if (!vb1 || !vb2) return;

		Entity* e1 = static_cast<Entity*>(vb1);
		Entity* e2 = static_cast<Entity*>(vb2);

		e1->touchEnded(e2, contact->GetFixtureA(), contact->GetFixtureB());
		e2->touchEnded(e1, contact->GetFixtureB(), contact->GetFixtureA());
	}

	bool BlobLayer::onTouchBegan(Touch* t, Event*)
	{
		if (state != GameStates::Playing) return false;

		auto touchLoc = convertToNodeSpace(t->getLocation());
		
		auto myring = MAKESPRITE("ring");
		myring->setOpacity(0);
		addChild(myring, 2);

		ring[t->getID()] = myring;
		ring[t->getID()]->stopAllActions();
		ring[t->getID()]->runAction(RepeatForever::create(Sequence::createWithTwoActions(ScaleTo::create(0.3f, 1.5f), ScaleTo::create(0.3f, 0.7f))));
		ring[t->getID()]->runAction(FadeIn::create(0.5f));
		ring[t->getID()]->setPosition(touchLoc);

		vector<Entity*> entitiesToDelete;
		for (auto e : entities)
		{
			if (e->dead) entitiesToDelete.push_back(e);
			else if (touchLoc.distance(e->getPosition()) < e->getContentSize().width)
			{
				if (e->bomb) e->explodeBomb();
				else e->makeHappy();
				break;
			}

		}
		for (auto e : entitiesToDelete) entities.erase(e);

		return true;
	}

	void BlobLayer::onTouchMoved(Touch* t, Event*)
	{
		auto touchLoc = convertToNodeSpace(t->getLocation());
		ring[t->getID()]->setPosition(touchLoc);
	}

	void BlobLayer::onTouchEnded(Touch* t, Event*)
	{
		if (ring.find(t->getID()) != ring.end())
		{
			ring[t->getID()]->removeFromParent();
			ring.erase(t->getID());
		}
	}

	void BlobLayer::checkCombo(float)
	{
		int combo = (BlobScene::LastScore - lastObservedScore) / 6;
		if (combo > 0)
		{
			auto color = Entity::Colors[combo % Entity::Colors.size()];
			flashBigText(LS("Combo") + "\nx" + nToString(combo+1), color);
			PlayEffect(SOUNDS_COMBO, (float) combo * 0.1f + 0.8f, 0.7f);
		}
		lastObservedScore = BlobScene::LastScore;
	}

	void BlobLayer::flashBigText(string s, Color3B c)
	{
		bigText->setString(s);
		bigText->setColor(c);
		bigText->stopAllActions();
		bigText->setOpacity(255);
		bigText->runAction(Sequence::createWithTwoActions(
			DelayTime::create(0.7f),
			FadeOut::create(0.3f)));
	}

	BlobLayer::~BlobLayer()
	{
		Entity::Game = nullptr;
		delete world;
	}
}