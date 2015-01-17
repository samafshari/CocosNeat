#include "Raid.h"
#include "ExtraGameOverScene.h"
#include "Ooze.h"

using namespace Raid;

const float LevelGenerator::RIBBON_WIDTH = 16.0f;
const float LevelGenerator::RIBBON_HEIGHT = 16.0f;
const int LevelGenerator::MAX_SECTION_OUTER_RIBBONS = 50;
const int LevelGenerator::MIN_SECTION_OUTER_RIBBONS = 5;
const int LevelGenerator::MIN_SECTIONS = 7;
const int LevelGenerator::MAX_SECTIONS = 14;
const int LevelGenerator::MAX_OUTER_BLOCKS = DESIGN_X * 0.9f * 0.5f / RIBBON_WIDTH;
const int LevelGenerator::MIN_OUTER_BLOCKS = DESIGN_X * 0.3f * 0.5f / RIBBON_WIDTH;
const int LevelGenerator::BRIDGE_WIDTH_BLOCKS = DESIGN_X * 0.3f * 0.5f / RIBBON_WIDTH;
const int LevelGenerator::BRIDGE_RIBBONS = 10;

const int LevelGenerator::ISLAND_THRESHOLD = 15;
const int LevelGenerator::MIN_ISLAND_GAP = 10;
const int LevelGenerator::MIN_ISLAND_LENGTH = 10;

const Size LevelGenerator::TREE_SIZE = Size(98, 69) * Entity::SCALE;
const Size LevelGenerator::HOUSE_SIZE = Size(185, 68) * Entity::SCALE;
const Size LevelGenerator::ENTITY_SIZE = Size(200, 205) * Entity::SCALE;

const int LevelGenerator::TREEHOUSE_CHANCE = 2;
const float LevelGenerator::TREEHOUSE_DISTANCE_MIN = PY(0.2f);
const float LevelGenerator::TREEHOUSE_DISTANCE_MAX = PY(1.0f);

vector<string> LevelGenerator::GenerateLevel(int levelId)
{
	srand(levelId);

	//Algorithm works like this (for outer walls):
	//each level is made of sections and transitions
	//sections have static height and width
	//transitions have static tooth width (step)
	//level starts with bridge size + transition to first section
	//level ends with transition to bridge section + bridge section

	vector<Point> outerWalls;
	vector<Point> innerWalls;
	vector<LevelEntityData> entities;

	if (levelId < 0)
	{
		outerWalls.push_back(Point(BRIDGE_WIDTH_BLOCKS * RIBBON_WIDTH, 0));
		outerWalls.push_back(Point(BRIDGE_WIDTH_BLOCKS * RIBBON_WIDTH, RIBBON_HEIGHT * BRIDGE_RIBBONS * 4));
		return ToString(outerWalls, innerWalls, entities);
	}

	//generate outer walls
	float y = 0;
	outerWalls.push_back(Point(BRIDGE_WIDTH_BLOCKS * RIBBON_WIDTH, y));
	auto beginSection = GenerateSection(BRIDGE_WIDTH_BLOCKS, BRIDGE_RIBBONS, y);
	outerWalls.insert(outerWalls.end(), beginSection.begin(), beginSection.end());
	int sections = rand() % (MAX_SECTIONS - MIN_SECTIONS) + MIN_SECTIONS;
	if (levelId % 4 == 0) sections = 1;
	int oldWidth = BRIDGE_WIDTH_BLOCKS;
	for (int i = 0; i < sections; i++)
	{
		int width = MIN_OUTER_BLOCKS + rand() % (MAX_OUTER_BLOCKS - MIN_OUTER_BLOCKS);
		int height = MIN_SECTION_OUTER_RIBBONS + rand() % (MAX_SECTION_OUTER_RIBBONS - MIN_SECTION_OUTER_RIBBONS);

		if (levelId % 4 == 0)
		{
			width = BRIDGE_WIDTH_BLOCKS * 2;
			height = MAX_SECTIONS * MAX_SECTION_OUTER_RIBBONS / 2;
		}

		auto transition = GenerateTransition(oldWidth, width, y);
		outerWalls.insert(outerWalls.end(), transition.begin(), transition.end());

		auto section = GenerateSection(width, height, y);
		outerWalls.insert(outerWalls.end(), section.begin(), section.end());

		oldWidth = width;
	}
	auto transition = GenerateTransition(oldWidth, BRIDGE_WIDTH_BLOCKS, y);
	outerWalls.insert(outerWalls.end(), transition.begin(), transition.end());
	auto endSection = GenerateSection(BRIDGE_WIDTH_BLOCKS, BRIDGE_RIBBONS, y);
	outerWalls.insert(outerWalls.end(), endSection.begin(), endSection.end());

	//generate inner walls
	if (levelId >= 1 && levelId % 4 > 0)
		innerWalls = GenerateInnerWalls(outerWalls);

	//simplify walls
	outerWalls = SimplifyWalls(outerWalls);
	innerWalls = SimplifyWalls(innerWalls);

	//add entities
	entities = GenerateEntities(outerWalls, innerWalls, levelId);
	auto leftHouses = GenerateOuterTreesAndHouses(outerWalls, true);
	auto rightHouses = GenerateOuterTreesAndHouses(outerWalls, false);
	auto innerHouses = GenerateInnerTreesAndHouses(innerWalls);

	entities.insert(entities.begin(), leftHouses.begin(), leftHouses.end());
	entities.insert(entities.begin(), rightHouses.begin(), rightHouses.end());
	entities.insert(entities.begin(), innerHouses.begin(), innerHouses.end());

	//add jets
	if (levelId > 4)
	{
		float y = 0;
		float height = (outerWalls.rbegin())->y;
		while (y > height + (BRIDGE_RIBBONS + ISLAND_THRESHOLD) * RIBBON_HEIGHT)
		{
			if (rand() % 5 == 0)
			{
				Point pos(PX(0.5f), y);
				bool flip = false;
				if (rand() % 2 == 0)
				{
					pos.x = -PX(0.5f);
					flip = true;
				}
				entities.push_back(LevelEntityData{ Items::Jet, pos, flip });
			}
			y -= MIN_ISLAND_GAP * RIBBON_HEIGHT;
		}
	}

	//add bridge
	entities.push_back(LevelEntityData{ Items::Bridge,
		Point(0, outerWalls.rbegin()->y), false });

	//log
	//for (auto line : data) log(line);
	return ToString(outerWalls, innerWalls, entities);
}

vector<string> LevelGenerator::ToString(vector<Point> outerWalls,
	vector<Point> innerWalls, vector<LevelEntityData> entities)
{
	//translate generated data into level format
	vector<string> data;
	data.push_back("~2");
	data.push_back("~inner");
	for (auto wall : innerWalls) data.push_back(nToString(wall.x) + "," + nToString(wall.y));
	data.push_back("~outer");
	for (auto wall : outerWalls) data.push_back(nToString(wall.x) + "," + nToString(wall.y));
	data.push_back("~entities");
	for (auto entity : entities)
	{
		data.push_back(nToString(entity.position.x) + "," + nToString(entity.position.y));
		data.push_back(nToString(entity.flip));
		data.push_back(nToString((int) entity.itype));
	}

	return data;
}

vector<Point> LevelGenerator::GenerateSection(int width, int height, float& y)
{
	if (height == 0) return vector<Point> {};
	y -= height * RIBBON_HEIGHT;
	return vector<Point>{ Point(width * RIBBON_WIDTH, y) };
}

vector<Point> LevelGenerator::GenerateTransition(int beginW, int endW, float& y)
{
	vector<Point> result;
	if (beginW == endW) return result;

	int step = 1;// +rand() % 2;

	int w = beginW;
	while (true)
	{
		if (endW > beginW)
		{
			w += step;
			if (w > endW) break;
		}
		if (endW < beginW)
		{
			w -= step;
			if (w < endW) break;
		}
		y -= RIBBON_HEIGHT;
		result.push_back(Point(w * RIBBON_WIDTH, y));
	}

	return result;
}

vector<Point> LevelGenerator::GenerateInnerWalls(vector<Point> outer)
{
	vector<Point> result;
	if (outer.size() == 0) return result;

	float height = (outer.rbegin())->y;
	float y = -(BRIDGE_RIBBONS + ISLAND_THRESHOLD) * RIBBON_HEIGHT;

	result.push_back(Point(0, y));
	while (y > height + (BRIDGE_RIBBONS + ISLAND_THRESHOLD) * RIBBON_HEIGHT)
	{
		int x = (result.rbegin()->x / RIBBON_WIDTH);
		int maxX = GetOuterWidth(outer, y) / RIBBON_WIDTH;
		int newX = rand() % 4;
		if (newX == 0)
		{
			newX = rand() % maxX;
			int islandLength = MIN_ISLAND_LENGTH + rand() % MIN_ISLAND_LENGTH;
			auto transition = GenerateInnerTransition(outer, x, newX, y);
			result.insert(result.end(), transition.begin(), transition.end());

			auto section = GenerateInnerSection(outer, newX, islandLength, y);
			result.insert(result.end(), section.begin(), section.end());

			auto transition2 = GenerateInnerTransition(outer, newX, 0, y);
			result.insert(result.end(), transition2.begin(), transition2.end());
		}
		else
		{
			if (x > 0)
			{
				auto transition = GenerateInnerTransition(outer, x, 0, y);
				result.insert(result.end(), transition.begin(), transition.end());
			}
			y -= MIN_ISLAND_GAP * RIBBON_HEIGHT;
			result.push_back(Point(0, y));
		}
	}
	int x = (result.rbegin()->x / RIBBON_WIDTH);
	if (x > 0)
	{
		auto transition = GenerateInnerTransition(outer, x, 0, y);
		result.insert(result.end(), transition.begin(), transition.end());
	}

	return result;
}

vector<Point> LevelGenerator::GenerateInnerSection(vector<Point> outer, int width, int height, float& y)
{
	vector<Point> result;
	float end = outer.rbegin()->y;
	if (y - height * RIBBON_HEIGHT < end + (BRIDGE_RIBBONS + ISLAND_THRESHOLD) * RIBBON_HEIGHT)
		height = fabs(y + (BRIDGE_RIBBONS + ISLAND_THRESHOLD + width)) / RIBBON_HEIGHT;
    
	for (int i = 0; i < height; i++)
	{
		int	cx = clamp(width, 0, GetOuterWidth(outer, y) / RIBBON_WIDTH - MIN_OUTER_BLOCKS) / 2;

		y -= RIBBON_HEIGHT;
		result.push_back(Point(cx * RIBBON_WIDTH, y));
	}
	return result;
}

vector<Point> LevelGenerator::GenerateInnerTransition(vector<Point> outer, int beginW, int endW, float& y)
{
	vector<Point> result;
	if (beginW == endW) return result;

	int step = 1;

	int w = beginW;
	while (true)
	{
		if (endW > beginW)
		{
			w += step;
			if (w > endW) break;
		}
		if (endW < beginW)
		{
			w -= step;
			if (w < endW) break;
		}

		int	cx = clamp(w, 0, GetOuterWidth(outer, y) / RIBBON_WIDTH - MIN_OUTER_BLOCKS) / 2;

		y -= RIBBON_HEIGHT;
		result.push_back(Point(cx * RIBBON_WIDTH, y));
	}

	return result;
}

vector<Point> LevelGenerator::SimplifyWalls(vector<Point> walls)
{
	vector<Point> result;
	if (walls.size() == 0) return result;
	auto itr = walls.begin();
	result.push_back(*itr);
	Point last = *itr;
	itr++;
	for (; itr != walls.end(); itr++)
	{
		if (last.x != itr->x)
		{
			last = *itr;
			result.push_back(last);
		}
	}
	if ((*result.rbegin()) != (*walls.rbegin())) result.push_back(*walls.rbegin());
	return result;
}

vector<LevelEntityData> LevelGenerator::GenerateOuterTreesAndHouses(vector<Point> outer, bool left)
{
	vector<LevelEntityData> result;
	if (outer.size() == 0) return result;

	float height = outer.rbegin()->y; //negative
	float y = height + BRIDGE_RIBBONS * RIBBON_HEIGHT / 2;
	while (y < BRIDGE_RIBBONS * RIBBON_HEIGHT / 2)
	{
		if (rand() % TREEHOUSE_CHANCE == 0)
		{
			float x = GetMaxWidth(outer, y + HOUSE_SIZE.height, y - HOUSE_SIZE.height);
			float treeX = x;

			if (left)
			{
				x += HOUSE_SIZE.width * RANDF2(1.2f, 1.5f);
				treeX += TREE_SIZE.width * RANDF2(1.2f, 1.5f);
			}
			else
			{
				x = -x - HOUSE_SIZE.width * RANDF2(0.1f, 1.0f);
				treeX = -treeX - TREE_SIZE.width * RANDF2(0.7f, 1.5f);
			}

			float treeY = (rand() % 2 == 0) ?
				y + HOUSE_SIZE.height * 1.1f :
				y - HOUSE_SIZE.height * 1.1f;

			result.push_back(LevelEntityData{ Items::House, Point(x, y), false });
			result.push_back(LevelEntityData{ Items::Tree, Point(treeX, treeY), false });
		}
		y += ENTITY_SIZE.height * 1.5f + RANDF2(0, TREEHOUSE_DISTANCE_MAX);
	}
	return result;
}

vector<LevelEntityData> LevelGenerator::GenerateInnerTreesAndHouses(vector<Point> inner)
{
	vector<LevelEntityData> result;
	if (inner.size() == 0) return result;

	float y = 0;
	for (auto itr = inner.begin(); itr != inner.end(); itr++)// p: inner)
	{
		if (itr->x > HOUSE_SIZE.width * 0.6f &&
			(itr + 1) != inner.end() &&
			(HOUSE_SIZE.height + TREE_SIZE.height) * 1.2f < fabs((itr + 1)->y - itr->y))
		{
			y = itr->y - HOUSE_SIZE.height * RANDF2(1.0f, 5.0f);
			while (y > (itr + 1)->y)
			{
				if (rand() % 2 == 0)
				{
					float width = GetMinWidth(inner, y + HOUSE_SIZE.height, y - HOUSE_SIZE.height);
					if (width * 2.0f > HOUSE_SIZE.width && rand() % max(1, (TREEHOUSE_CHANCE / 2)) == 0)
					{
						float x = RANDF2(-width + HOUSE_SIZE.width, width);
						result.push_back(LevelEntityData{ Items::House, Point(x, y), false });
					}
					else
					{
						width = GetMinWidth(inner, y + TREE_SIZE.height, y - TREE_SIZE.height);
						if (width * 2.0f > TREE_SIZE.width && rand() % max(1, (TREEHOUSE_CHANCE / 2)) == 0)
						{
							float x = RANDF2(-width + TREE_SIZE.width, width);
							result.push_back(LevelEntityData{ Items::Tree, Point(x, y), false });
						}
					}
				}
				else
				{
					float width = GetMinWidth(inner, y + TREE_SIZE.height, y - TREE_SIZE.height);
					if (width * 2.0f > TREE_SIZE.width && rand() % max(1, (TREEHOUSE_CHANCE / 2)) == 0)
					{
						float x = RANDF2(-width + TREE_SIZE.width, width);
						result.push_back(LevelEntityData{ Items::Tree, Point(x, y), false });
					}
				}
				y -= RANDF2(TREEHOUSE_DISTANCE_MIN, TREEHOUSE_DISTANCE_MAX) * 0.15f;
			}
		}
	}
	return result;
}

vector<LevelEntityData> LevelGenerator::GenerateEntities(vector<Point> outer, vector<Point> inner, int level)
{
	vector<LevelEntityData> result;
	if (outer.size() == 0) return result;

	float height = outer.rbegin()->y; //negative
	float y = height + BRIDGE_RIBBONS * RIBBON_HEIGHT / 2;
	while (y < -BRIDGE_RIBBONS * RIBBON_HEIGHT / 2)
	{
		float x = GetMinWidth(outer, y + RIBBON_HEIGHT * 4, y - RIBBON_HEIGHT * 4);
		float e = 0;
		if (inner.size() > 0) e = GetMaxWidth(inner, y + RIBBON_HEIGHT * 4, y - RIBBON_HEIGHT * 4);
		if (e != 0)
		{
			if (rand() % 2 == 0) x = -RANDF2(e, x - ENTITY_SIZE.width); //right
			else x = RANDF2(x - ENTITY_SIZE.width * 0.5f, e + ENTITY_SIZE.width); //left
		}
		else x = RANDF2(-x + ENTITY_SIZE.width, x - ENTITY_SIZE.width * 0.5f);

		//choose item
		vector<Items> items = {
			Items::Heli, Items::Heli, Items::Heli,
			Items::Ship, Items::Ship,
			Items::Fuel
		};
		Items item = CHOOSE(items);
		bool flip = item != Items::Fuel && rand() % 2 == 0;

		result.push_back(LevelEntityData{ item, Point(x, y), flip });
		float coef = 0.75f;
		if (level > 2) coef = 0.6f;
		if (level > 3) coef = 0.5f;
		if (level > 4) coef = 0.4f;
		if (level > 6) coef = 0.3f;
		if (level > 8) coef = 0.25f;
		y += RANDF2(TREEHOUSE_DISTANCE_MIN, TREEHOUSE_DISTANCE_MAX) * coef;
	}

	return result;
}

float LevelGenerator::GetOuterWidth(vector<Point> outer, float y, int delta)
{
	//y is negative
	return GetMinWidth(outer, y + RIBBON_HEIGHT * delta, y - RIBBON_HEIGHT * delta);
}

float LevelGenerator::GetMinWidth(vector<Point> wall, float yBegin, float yEnd)
{
	//y is negative
	float lastX = wall[0].x;
	auto itr = wall.begin();
	for (; itr != wall.end(); itr++)
	{
		if (itr->y < yBegin) break;
		lastX = itr->x;
	}
	for (; itr != wall.end(); itr++)
	{
		if (itr->y < yEnd) break;
		lastX = min(lastX, itr->x);
	}

	return lastX;
}

float LevelGenerator::GetMaxWidth(vector<Point> wall, float yBegin, float yEnd)
{
	float lastX = wall[0].x;
	auto itr = wall.begin();
	for (; itr != wall.end(); itr++)
	{
		if (itr->y < yBegin) break;
		lastX = itr->x;
	}
	for (; itr != wall.end(); itr++)
	{
		if (itr->y < yEnd) break;
		lastX = max(lastX, itr->x);
	}

	return lastX;
}