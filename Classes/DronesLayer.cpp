#include "DronesLayer.h"
#include "helpers.h"
#include "DronesScene.h"

using namespace Drones;

const float DronesLayer::blockSize = DESIGN_X / (float) SCREENXCOUNT;

DronesLayer* DronesLayer::create(DronesScene* _scene, vector<string> levelData)
{
	DronesLayer* game = new DronesLayer();
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
	game->forceNoScroll = false;
	game->parseLevel(levelData);

	game->gameSize = Size(blockSize * (float) (Position::width), blockSize * (float) (Position::height - 1));
	game->setContentSize(game->gameSize);
	//auto lc = LayerColor::create(Color4B(255, 0, 0, 100), game->gameSize.width, game->gameSize.height);
	//game->addChild(lc, 1000);
	game->initAllEntities();

	game->switchToReady();
	log("Game size is: " + nToString(game->gameSize.width) + "," + nToString(game->gameSize.height));
	log("Design size is: " + nToString(DESIGN_X) + "," + nToString(DESIGN_Y));
	return game;
}

bool DronesLayer::init()
{
	if (!Layer::init()) return false;
	moves = 0;
	overlay = Layer::create();
	addChild(overlay, 1);
	
	auto statusLayer = Layer::create(); // LayerColor::create(Color4B::BLACK, DESIGN_X, PY(0.08f));
	addChild(statusLayer, 10);
	statusLayer->setPosition(CP(0.0, -0.1f));

	return true;
}

void DronesLayer::parseLevel(vector<string> levelData)
{
	//if (!levelData.size()) return;

	int y = 0;
	player = nullptr;
	monsters = 0;
	kills = 0;
	collectedGems = 0;
	eggplants = 0;
	lollis = 0;

	Entity* selE = nullptr;
    message = "";
	Position::width = static_cast<int>(levelData[0].size());
	Position::height = -1;
	bool parseTiles = true;
	log("Begin reading map");
	for (string line : levelData)
	{
		if (line != "ENDGRID" && parseTiles)
		{
			Position::height++;
			log(line);
			assert(static_cast<int>(line.size()) == Position::width);
			for (int x = 0; x < Position::width; x++)
			{
				char charcode = line[x];
				Entity* e = makeEntity(charcode);
				if (e != nullptr)
				{
					if (e->player) 
						player = static_cast<Player*>(e);

					Position pos{ x, y };

					int zorder = 0;
					if (!dynamic_cast<Fake*>(e))
						addEntity(e, pos, zorder + e->initZ);
					if (dynamic_cast<Robot*>(e)) monsters++;
					if (dynamic_cast<Eggplant*>(e)) eggplants++;
					//add fake
					auto fake = Fake::create();
					fake->sprite = fake->AddSprite("repeat");
					if ((pos.x + (pos.y * Position::width)) % 2 == 0)
						fake->setDefaultColor(200, 200, 200);
					fake->initZ = -5;
					//addEntity(fake, pos, zorder + fake->initZ);
				}
			}
			y++;
			continue;
		}
		else if (line == "ENDGRID") Position::height++;
		
		parseTiles = false;
		y++;

		int cpos = 0;
		string cmd = ReadBlock(line, cpos, ' ');
		if (cmd == "pos")
		{
			string xStr = ReadBlock(line, cpos, ',');
			string yStr = ReadBlock(line, cpos);
			int selX = nToInt(xStr);
			int selY = nToInt(yStr);
			assert(selX >= 0 && selX < Position::width && selY >= 0 && selY < Position::height);
			Position p = { selX, selY };
			auto entities = getEntities(p);
			assert(entities.size());
			selE = entities[0];
		}
		else if (cmd == "pval")
		{
			//int val = nToInt(ReadBlock(line, cpos));
		}
		else if (cmd == "scale")
		{
			float val = nToFloat(ReadBlock(line, cpos));
			setScale(val);
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
void DronesLayer::pickEggplant()
{
	eggplants--;
	GameSettings::DronesLastScore++;
	scene->updateRandomStats();
	if (eggplants <= 0)
	{
		GameSettings::DronesLastScore += 5;
		gameOver(true);
	}
}

void DronesLayer::pickLolli()
{
	lollis++;
	GameSettings::DronesLastScore += 5;
	scene->updateRandomStats();
}

void DronesLayer::lolliDied()
{
}

void DronesLayer::spawnBonus(float)
{/*
	if (rand() % 2 != 0)
	{
		log("Spawn bonus was unlocky");
		return;
	}*/

	Position p;
	do p = findSafePosition(); while (p.x < 0 || p.y < 0);
	auto bonus = makeEntity('g'); // '%');
	addEntity(bonus, p);
	initEntity(bonus);
	log("Spawn bonus at position " + nToString(p.x) + "," + nToString(p.y));
}

Position DronesLayer::findSafePosition()
{
	for (int i = 0; i < 50; i++)
	{
		Position p{ rand() % Position::width, rand() % (Position::height - 1) };
		if (getEntities(p).size() == 0) return p;
	}

	//safe place not found after N tries. return fail.
	return Position{ -1, -1 };
}

vector<Entity*> DronesLayer::getEntities(Position p)
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

bool DronesLayer::move(Entity* e, Directions d, Position* dest, bool simulation, float animDuration)
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

void DronesLayer::teleport(Entity* e, Position pos)
{
	e->stopAllActions();
	e->position.x = pos.x;
	e->position.y = pos.y;
	e->setPosition(Point(blockSize * e->position.x, blockSize * e->position.y));
	e->sprite->runAction(Sequence::createWithTwoActions(
		FadeOut::create(0.1f), FadeIn::create(0.5f)
		));
}

void DronesLayer::broadcast(Entity* e, const string& msg)
{
	if (!this || state != GameStates::Ready) return;

	for (Entity* subscriber : entities)
	{
		if (subscriber->listener && !subscriber->dead && subscriber != e)
			subscriber->receiveMessage(e, msg);
	}
}

void DronesLayer::addEntity(Entity* e, Position pos, int zorder)
{
	addChild(e, zorder);
	e->position.x = pos.x;
	e->position.y = pos.y;
	entities.push_back(e);
}

void DronesLayer::initEntity(Entity* e)
{
	if (doTranspose)
		swap(e->position.x, e->position.y);
	if (!doFlipY)
		e->position.y = Position::height - e->position.y - 2;
	if (doFlipX)
		e->position.x = Position::width - 1 - e->position.x;
	
	e->setPositionX(e->position.x * blockSize);
	e->setPositionY(e->position.y * blockSize);

	e->startGameCall();
}

void DronesLayer::initAllEntities()
{
	for (Entity* e : entities)
	{
		initEntity(e);
	}
}

void DronesLayer::removeEntity(Entity* e)
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

void DronesLayer::switchToReady(float)
{
	state = GameStates::Ready;
}

void DronesLayer::gameOver(bool win)
{
	state = GameStates::Done;
	didWin = win;
	scheduleOnce(schedule_selector(DronesLayer::doGameOver), 1.0f);
}

void DronesLayer::doGameOver(float)
{
	if (didWin)
	{
		GameSettings::GiveGems(collectedGems);
		int oldMinedGems = GameSettings::MinedGems;
		GameSettings::MinedGems += collectedGems;
		if (GameSettings::MinedGems >= 50 && oldMinedGems < 50)
			GameSettings::ReportAchievement(ACHIEVEMENT_GEM_MINE);
	}
	GameSettings::SaveSettings();
	unscheduleAllSelectors();
	Entity::Game = nullptr;
	scene->advanceLevel();
}

void DronesLayer::spawnPlayer(Position p)
{
	player = static_cast<Player*>(makeEntity('@'));
	addEntity(player, p, -1);
	log("Added player at position " + nToString(p.x) + "," + nToString(p.y));
}

void DronesLayer::kill(Entity* e)
{
	removeEntity(e);
}

void DronesLayer::startCutscene(Ref* sender)
{
	state = GameStates::Cutscene;
	for (auto e : entities) e->pauseCall();
}

Point DronesLayer::positionToPoint(Position p)
{
	return Point(blockSize * p.x, blockSize * p.y);
}

Entity* DronesLayer::makeEntity(char charcode)
{
	return Entity::MakeEntity(charcode);
}

void DronesLayer::clearMap()
{
	for (int i = 0; i < (int) entities.size(); removeEntity(entities[i++]));
}

void DronesLayer::playerDied()
{
	GameSettings::SaveSettings();
	gameOver(false);
}
