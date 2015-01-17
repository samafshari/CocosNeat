#include <functional>

#include "PlatformerLayer.h"
#include "helpers.h"
#include "PlatformerScene.h"

namespace Platformer
{
	const float PlatformerLayer::blockSize = DESIGN_X / (float) SCREENXCOUNT;

	PlatformerLayer* PlatformerLayer::create(PlatformerScene* _scene, vector<string> levelData)
	{
		Size visibleSize = Director::getInstance()->getVisibleSize();
		PlatformerLayer* game = new PlatformerLayer();
		game->scene = _scene;

		if (!game || !game->init())
		{
			CC_SAFE_DELETE(game);
			return NULL;
		}

		Entity::Game = game;
		game->autorelease();

		game->doFlipX = false;
		game->doFlipY = false;
		game->doTranspose = false;
		game->parseLevel(levelData);

		game->initAllEntities();

		game->forceNoScroll = false;
		
		game->switchToReady();

		log("Game size is: " + nToString(visibleSize.width) + "," + nToString(visibleSize.height));
		log("Visible size is: " + nToString(DESIGN_X) + "," + nToString(DESIGN_Y));
		game->setPositionX((visibleSize.width - game->gameSize.width) * 0.5f);
		game->setPositionY((visibleSize.height - game->gameSize.height) * 0.5f);
		return game;
	}

	bool PlatformerLayer::init()
	{
		if (!Layer::init()) return false;
		moves = 0;
		monsters = 0;
		eggplants = 0;
		kills = 0;
		initGravityScale = 1.0f;

		overlay = Layer::create();
		addChild(overlay, 1);

		auto statusLayer = Layer::create(); // LayerColor::create(Color4B::BLACK, DESIGN_X, PY(0.08f));
		addChild(statusLayer, 10);
		statusLayer->setPosition(CP(0.0, -0.1f));

		return true;
	}

	Entity* PlatformerLayer::ParseAddEntity(char charcode, Position pos)
	{
		Entity* e = makeEntity(charcode);
		if (e != nullptr)
		{
			if (e->player)
				player = static_cast<Player*>(e);

			int zorder = 0;
			if (!dynamic_cast<Fake*>(e))
				addEntity(e, pos, zorder + e->initZ);
			if (dynamic_cast<Monster*>(e)) monsters++;
			if (dynamic_cast<Eggplant*>(e)) eggplants++;
			//add fake
			auto fake = Fake::create();
			fake->sprite = fake->AddSprite("repeat");
			if ((pos.x + (pos.y * Position::width)) % 2 == 0)
				fake->setDefaultColor(200, 200, 200);
			fake->initZ = -5;
		}
		return e;
	}

	void PlatformerLayer::parseLevel(vector<string> levelData)
	{
		int y = 0;
		player = nullptr;
		Entity* selE = nullptr;
		message = "";
		
		Position::height = -1;
		bool parseTiles = false;// true;
		Color4B selColor = Color4B::WHITE;
		int selX = 0, selY = 0;
		char selChar = '.';
		Point selPt = Point::ZERO;
		Size scale = Size(1, 1);
		bool bg_anim = false;

		log("Begin reading map");
		for (string line : levelData)
		{
			if (line == "BEGINGRID")
			{
				parseTiles = true;
				continue;
			}
			if (line != "ENDGRID" && parseTiles)
			{
				if (y==0) Position::width = static_cast<int>(line.size());
				Position::height++;
				log(line);
				assert(static_cast<int>(line.size()) == Position::width);
				for (int x = 0; x < Position::width; x++)
				{
					char charcode = line[x];
					Position pos{ x+1, y };
					ParseAddEntity(charcode, pos);
				}
				y++;
				continue;
			}
			else if (line == "ENDGRID") Position::height++;

			parseTiles = false;
			//y++;

			int cpos = 0;
			string cmd = ReadBlock(line, cpos, ' ');
			if (cmd == "pick")
			{
				string xStr = ReadBlock(line, cpos, ',');
				string yStr = ReadBlock(line, cpos);
				selX = nToInt(xStr) - 1;
				selY = nToInt(yStr) - 1;
				assert(selX >= 0 && selX < Position::width && selY >= 0 && selY < Position::height);
				Position p = { selX, selY };
				auto entities = getEntities(p);
				assert(entities.size());
				selE = entities[0];
			}
			else if (cmd == "pos")
			{
				string xStr = ReadBlock(line, cpos, ',');
				string yStr = ReadBlock(line, cpos);
				selX = nToInt(xStr) - 1;
				selY = nToInt(yStr) - 1;
				selPt = Point(selX * blockSize + blockSize,
					(Position::height - selY - 2) * blockSize + blockSize);
				if (doFlipY) selPt.y = selY * blockSize + blockSize;
				log("Position selected: " + nToString(selX) + "," + nToString(selY));
			}
			else if (cmd == "xy")
			{
				string xStr = ReadBlock(line, cpos, ',');
				string yStr = ReadBlock(line, cpos);
				selPt.x = nToFloat(xStr);
				selPt.y = nToFloat(yStr);
				log("XY selected: " + nToString(selPt));
			}
			else if (cmd == "put")
			{
				string cStr = ReadBlock(line, cpos);
				selChar = cStr[0];
				Entity* e = ParseAddEntity(selChar, Position{ -1, -1 });
				if (e)
				{
					e->setPosition(selPt);
				}
				log("Entity " + nToString(selChar) + " put at: " + nToString(selPt));
			}
			else if (cmd == "scale")
			{
				string xStr = ReadBlock(line, cpos, ',');
				string yStr = ReadBlock(line, cpos);
				scale = Point(nToFloat(xStr), nToFloat(yStr));
				log("Scale set: " + nToString(selX) + "," + nToString(selY));
			}
			else if (cmd == "gravity")
			{
				string gStr = ReadBlock(line, cpos);
				initGravityScale = nToFloat(gStr);
				log("Gravity set: " + nToString(initGravityScale));
			}
			else if (cmd == "rgba")
			{
				string rStr = ReadBlock(line, cpos, ',');
				string gStr = ReadBlock(line, cpos, ',');
				string bStr = ReadBlock(line, cpos, ',');
				string aStr = ReadBlock(line, cpos);
				selColor.r = nToInt(rStr);
				selColor.g = nToInt(gStr);
				selColor.b = nToInt(bStr);
				selColor.a = nToInt(aStr);
				log("Color selected: " +
					nToString(selColor.r) + "," +
					nToString(selColor.g) + "," +
					nToString(selColor.b) + "," +
					nToString(selColor.a));
			}
			else if (cmd == "rgb")
			{
				string rStr = ReadBlock(line, cpos, ',');
				string gStr = ReadBlock(line, cpos, ',');
				string bStr = ReadBlock(line, cpos);
				selColor.r = nToInt(rStr);
				selColor.g = nToInt(gStr);
				selColor.b = nToInt(bStr);
				selColor.a = 255;
				log("Color selected: " +
					nToString(selColor.r) + "," +
					nToString(selColor.g) + "," +
					nToString(selColor.b) + "," +
					nToString(selColor.a));
			}
			else if (cmd == "text")
			{
				string sizeStr = ReadBlock(line, cpos, ' ');
				string text = ReadBlock(line, cpos);
				float size = nToFloat(sizeStr);
				Label* l = Label::createWithTTF(text, FANCY_TTF, size);
				l->setAnchorPoint(Point(0.0f, 1.0f));
				l->setColor(Color3B(selColor.r, selColor.g, selColor.b));
				l->setOpacity(selColor.a);
				l->setPosition(selPt);
				l->setHorizontalAlignment(TextHAlignment::LEFT);
				l->setScaleX(scale.width);
				l->setScaleY(scale.height);
				addChild(l, -10);
			}
			else if (cmd == "img")
			{
				string fname = ReadBlock(line, cpos);
				Sprite* s = Sprite::create(fname);
				s->setAnchorPoint(Point(0.0f, 1.0f));
				s->setColor(Color3B(selColor.r, selColor.g, selColor.b));
				s->setOpacity(selColor.a);
				s->setPosition(selPt);
				s->setScaleX(scale.width);
				s->setScaleY(scale.height);
				addChild(s, -10);
			}
			else if (cmd == "background")
			{
				string fname = ReadBlock(line, cpos);
				static_cast<PlatformerScene*>(scene)->addBackground(
					fname, bg_anim, Color3B(selColor.r, selColor.g, selColor.b));
			}
			else if (cmd == "pval")
			{
				//int val = nToInt(ReadBlock(line, cpos));
			}
			else if (cmd == "message")
			{
				message = ReadBlock(line, cpos);
			}
			else if (cmd == "flipx")
				doFlipX = true;
			else if (cmd == "flipy")
				doFlipY = true;
			else if (cmd == "transpose")
				doTranspose = true;
			else if (cmd == "noscroll")
				forceNoScroll = true;
			else if (cmd == "bg_anim")
				bg_anim = true;
			else if (cmd == "zoom")
			{
				string xStr = ReadBlock(line, cpos, ',');
				string yStr = ReadBlock(line, cpos);
				setScale(nToFloat(xStr), nToFloat(yStr));
				log("Zoom set: " + nToString(selX) + "," + nToString(selY));
			}
			else if (selE)
			{
				selE->command(line);
			}
		}

		Position::height++;
		//assert(Position::height >= SCREENYCOUNT);

		if (player == nullptr)
		{
			log("Player not found.");
			assert(false);
		}
	}

	string PlatformerLayer::dumpLevel()
	{
		string result = "## Level Dumped at " + nToString((int)time(NULL)) + "\n";
		for (auto entity : entities)
		{
			result += "xy " + nToString(entity->getPosition()) + "\n";
			result += "put " + nToString(entity->charcode) + "\n";
		}
		//TODO: Export images and text

		result += "\n## End level dump.";
		return result;
	}

	void PlatformerLayer::pickEggplant()
	{
		eggplants--;
		GameSettings::DronesLastScore++;
		if (eggplants <= 0)
		{
			gameOver(true);
		}
	}

	vector<Entity*> PlatformerLayer::getEntities(Position p)
	{
		vector<Entity*> v;

		bool foundPlayer = false;
		//Illegal coords = no objects
		if (p.x < 0 || p.y < 0 || p.x >= Position::width || p.y >= Position::height)
		{
			assert(false);
			return v;
		}

		for (auto entity : entities)
		{
			if (entity->player && !entity->dead)
				foundPlayer = true;
			if (entity->position.x == p.x && entity->position.y == p.y && !entity->dead)
				v.push_back(entity);
		}

		//if (state == GameStates::Ready)
		//	assert(foundPlayer);

		return v;
	}

	void PlatformerLayer::move(Point p)
	{
		if (gravityFlipped()) p.y *= -1.0f;
		player->move(p);
	}

	bool PlatformerLayer::move(Entity* e, Directions d, Position* dest, bool simulation, float animDuration)
	{
		if (state != GameStates::Cutscene && state != GameStates::Ready) return false;

		//initiation
		NASSERT(e != nullptr);
		Position src = e->position;
		Position junk;
		if (dest == nullptr) dest = &junk;
		*dest = src;
		if (d == Directions::Stop)
		{
			e->moved(src);
			broadcast(e, EMSG_MOVED);
			return true;
		}

		//calculate dest position
		if (d == Directions::Left) dest->x--;
		else if (d == Directions::Right) dest->x++;
		else if (d == Directions::Up) dest->y++;
		else if (d == Directions::Down) dest->y--;
		else if (d == Directions::UL) { dest->x--; dest->y++; }
		else if (d == Directions::DL) { dest->x--; dest->y--; }
		else if (d == Directions::UR) { dest->x++; dest->y++; }
		else if (d == Directions::DR) { dest->x++; dest->y--; }

		//see if new position is inside
		if (dest->x < 0 || dest->y < 0 ||
			dest->x >= Position::width || dest->y >= Position::height - 1)
		{
			*dest = src;
			return false;
		}

		//see if object is free to move
		auto blockers = getEntities(*dest);

		if (!simulation)
		{
			bool problem = false;
			//real mode
			for (auto blocker : blockers)
			{
				bool answer = blocker->touch(e);
				if (!answer && !(blocker->pushable && e->pusher))
				{
					problem = true;
					continue;
				}

				//until here, touch answer was positive.
				//let's test for pushable
				if (blocker->pushable && e->pusher)
				{
					if (!move(blocker, d, nullptr, simulation, animDuration))
					{
						problem = true;
						continue;
					}
				}
			}

			if (problem)
			{
				*dest = src;
				return false;
			}
			if (e->player) moves++;
			e->moveTo(Point(blockSize * dest->x, blockSize * dest->y), animDuration);

			e->position.x = dest->x;
			e->position.y = dest->y;

			//send move signal to subscribers
			if (e->dead)
			{
				return false;
			}
			e->moved(src);
			broadcast(e, EMSG_MOVED);

		}
		else
		{
			//simulation mode
			for (auto blocker : blockers)
			{
				bool answer = blocker->potentialPassable(e);
				if (!answer && !blocker->pushable && !e->pusher)
					return false;
			}
		}

		return true;
	}

	void PlatformerLayer::teleport(Entity* e, Point pos)
	{
		teleportQueue.push_back({ e, pos });
	}

	void PlatformerLayer::broadcast(Entity* e, const string& msg)
	{
		if (!this || state != GameStates::Ready) return;

		for (Entity* subscriber : entities)
		{
			if (subscriber->listener && !subscriber->dead && subscriber != e)
				subscriber->receiveMessage(e, msg);
		}
	}

	void PlatformerLayer::addEntity(Entity* e, Position pos, int zorder)
	{
		entities.push_back(e);
		addChild(e, zorder);

		e->position.x = pos.x;
		e->position.y = pos.y;
	}

	void PlatformerLayer::initEntity(Entity* e)
	{
		if (e->position.x != -1 && e->position.y != -1)
		{
			if (doTranspose)
				swap(e->position.x, e->position.y);
			if (!doFlipY)
				e->position.y = Position::height - e->position.y - 2;
			if (doFlipX)
				e->position.x = Position::width - 1 - e->position.x;

			e->setPositionX(e->position.x * blockSize);// + blockSize);
			e->setPositionY(e->position.y * blockSize);// + blockSize);
		}

		auto aabb = e->getBoundingBox();
		bool dirty = false;
		if (gameSize.width < aabb.getMaxX())
		{
			gameSize.width = aabb.getMaxX();
			dirty = true;
		}
		if (gameSize.height < aabb.getMaxY())
		{
			gameSize.height = aabb.getMaxY();
			dirty = true;
		}
		if (dirty)
		{
			//gameSize.height += blockSize;
			setContentSize(gameSize);
		}

		e->createPhysics();
		e->startGameCall();
	}

	void PlatformerLayer::initAllEntities()
	{
		b2Vec2 gravity;
		gravity.Set(0.0f, -18.0f * initGravityScale);
		world = new b2World(gravity);
		world->SetContinuousPhysics(true);
		world->SetContactListener(this);

		// Create edges around the entire screen
		// Define the ground body.
		//b2BodyDef groundBodyDef;
		//groundBodyDef.position.Set(0, 0); // bottom-left corner

		//// Call the body factory which allocates memory for the ground body
		//// from a pool and creates the ground box shape (also from a pool).
		//// The body is also added to the world.
		//auto _groundBody = world->CreateBody(&groundBodyDef);

		//// Define the ground box shape.
		//b2EdgeShape groundBox;
		//// bottom
		//groundBox.Set(b2Vec2(0, 0), b2Vec2(DESIGN_X, 0));
		//_groundBody->CreateFixture(&groundBox, 0);

		//// top
		//groundBox.Set(b2Vec2(0, DESIGN_Y), b2Vec2(DESIGN_X, DESIGN_Y));
		//_groundBody->CreateFixture(&groundBox, 0);

		//// left
		//groundBox.Set(b2Vec2(0, DESIGN_Y), b2Vec2(0, 0));
		//_groundBody->CreateFixture(&groundBox, 0);

		//// right
		//groundBox.Set(b2Vec2(DESIGN_X, DESIGN_Y), b2Vec2(DESIGN_X, 0));
		//_groundBody->CreateFixture(&groundBox, 0);

		for (Entity* e : entities)
		{
			initEntity(e);
		}

		scheduleUpdate();
	}

	void PlatformerLayer::removeEntity(Entity* e)
	{
		if (e == nullptr) return;

		//doesn't compile on ndk
		/*auto itr = find(entities.begin(), entities.end(), e);
		assert(itr != entities.end());
		entities.erase(itr);*/

		for (int i = 0; i < static_cast<int>(entities.size()); i++)
		{
			if (entities[i] == e)
			{
				entities.erase(entities.begin() + i);
				break;
			}
		}

		removeChild(e);
	}

	void PlatformerLayer::switchToReady(float)
	{
		state = GameStates::Ready;
	}

	void PlatformerLayer::gameOver(bool win)
	{
		state = GameStates::Done;
		didWin = win;
		GameSettings::SaveSettings();
		unschedule(schedule_selector(PlatformerLayer::doGameOver));
		scheduleOnce(schedule_selector(PlatformerLayer::doGameOver), 1.0f);
	}

	void PlatformerLayer::doGameOver(float)
	{
		unscheduleAllSelectors();
		Entity::Game = nullptr;
		scene->advanceLevel();
	}

	void PlatformerLayer::spawnPlayer(Position p)
	{
		player = static_cast<Player*>(makeEntity('@'));
		addEntity(player, p, -1);
		log("Added player at position " + nToString(p.x) + "," + nToString(p.y));
	}

	void PlatformerLayer::kill(Entity* e)
	{
		removeEntity(e);
	}

	void PlatformerLayer::flipGravity(bool doFlip)
	{
		auto g = world->GetGravity();
		auto g2 = g;
		g2.y = fabs(g.y);
		if (!doFlip) g2.y *= -1;
		if (g2.y != g.y)
		{
			world->SetGravity(g2);
			for (auto entity : entities)
				if (!entity->dead) entity->gravityChanged(g2.y > 0);
			for (auto node : getChildren())

				if (dynamic_cast<Label*>(node) || dynamic_cast<Sprite*>(node))
				{
				auto ap = node->getAnchorPoint();
				ap.x = 1.0f - ap.x;
				ap.y = 1.0f - ap.y;
				node->setAnchorPoint(ap);
					node->setScaleY(-node->getScaleY());
					node->setScaleX(-node->getScaleX());
				}

			world->SetAllowSleeping(false);
		}
	}

	bool PlatformerLayer::gravityFlipped()
	{
		return world->GetGravity().y > 0;
	}

	void PlatformerLayer::startCutscene(Ref* sender)
	{
		state = GameStates::Cutscene;
		for (auto e : entities) e->pauseCall();
	}

	Point PlatformerLayer::positionToPoint(Position p)
	{
		return Point(blockSize * p.x, blockSize * p.y);
	}

	Entity* PlatformerLayer::makeEntity(char charcode)
	{
		return Entity::MakeEntity(charcode);
	}

	void PlatformerLayer::clearMap()
	{
		for (int i = 0; i < (int) entities.size(); removeEntity(entities[i++]));
	}

	void PlatformerLayer::playerDied()
	{
		GameSettings::SaveSettings();
		gameOver(false);
	}

	void PlatformerLayer::update(float dt)
	{
		if (state == GameStates::Ready ||
			state == GameStates::Editor) 
			updatePhysics(dt);
	}

	void PlatformerLayer::updatePhysics(float dt)
	{
		const int velocityIterations = 2;
		const int positionIterations = 2;

		// Instruct the world to perform a single step of simulation. It is
		// generally best to keep the time step and iterations fixed.
		if (state == GameStates::Ready)
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
			}
		}

		for (auto teleportData : teleportQueue)
		{
			teleportData.entity->stopAllActions();

			teleportData.entity->setPosition(teleportData.target);
			teleportData.entity->syncBodyPosition();
			teleportData.entity->body->SetLinearVelocity(
				-teleportData.entity->body->GetLinearVelocity());
			teleportData.entity->sprite->runAction(Sequence::createWithTwoActions(
				FadeOut::create(0.1f), FadeIn::create(0.5f)
				));
		}
		teleportQueue.clear();
	}

	void PlatformerLayer::BeginContact(b2Contact* contact)
	{
		if (state != GameStates::Ready) return;

		if (!contact->GetFixtureA() || !contact->GetFixtureB()) return;
		void* vb1 = contact->GetFixtureA()->GetBody()->GetUserData();
		void* vb2 = contact->GetFixtureB()->GetBody()->GetUserData();
		if (!vb1 || !vb2) return;

		Entity* e1 = static_cast<Entity*>(vb1);
		Entity* e2 = static_cast<Entity*>(vb2);

		if (e1->dead || e2->dead)
		{
			//contact->SetEnabled(false);
			return;
		}
		e1->touch(e2, contact->GetFixtureA(), contact->GetFixtureB());
		e2->touch(e1, contact->GetFixtureB(), contact->GetFixtureA());

		b2ContactListener::BeginContact(contact);
	}

	void PlatformerLayer::EndContact(b2Contact* contact)
	{
		if (state != GameStates::Ready) return;

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

	PlatformerLayer::~PlatformerLayer()
	{
		delete world;
	}
}