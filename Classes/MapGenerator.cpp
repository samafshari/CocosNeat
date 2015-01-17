#include "MapGenerator.h"

using namespace std;

#define RANDINT(min, max) (min + rand() % (1 + max - min))

const MapGenerator::MapMeta GAME_OF_DRONES_MAPMETA =
{
	13, //width
	13, //height
	{ //codes (code, min, max)
		{ '@', 1, 1 },
		{ 'X', 1, 1 },
		{ '8', 5, 20 },
		{ 'R', 1, 10 },
		{ '!', 2, 20 }
	},
	0
};

const MapGenerator::MapMeta EGGPLANT_MAPMETA =
{
	13,
	14,
	{
		{ '@', 1, 1 },
		{ '8', 2, 10 },
		{ 's', 10, 10 },
		{ '+', 10, 15 }
	},
	0
};

map<Position, char> MapGenerator::Generate(MapMeta meta)
{
	map<Position, char> level;
	Position::width = meta.width;
	Position::height = meta.height;
	Position playerPos;
	for (int x = 0; x < meta.width; x++)
	{
		for (int y = 0; y < meta.height; y++)
		{
			if (x == 0 || y == 0 || x == meta.width - 1 || y == meta.height - 1)
				level[{x, y}] = '8';
			else
				level[{x, y}] = '.';
		}
	}
	for (auto code : meta.codes)
	{
		int c = code.minCount;
		if (code.maxCount > code.minCount)
			c += rand() % (1 + code.maxCount - code.minCount);
		while (c > 0)
		{
			Position p;
			p.x = RANDINT(1, meta.width - 2);
			p.y = RANDINT(1, meta.height - 2);
			if (level[p] != '.') continue;
			level[p] = code.code;
			if (code.code == '@') playerPos = p;
			c--;
		}
	}

	return level;
}

Position MapGenerator::AddItem(char c, map<Position, char>& level, int minDist, int width, int height)
{
	//const int minDist = meta.width / 3;
	//find playerPos
	Position playerPos{ -1, -1 };
	for (int x = 0; x < width; x++)
	{
		for (int y = 0; y < height; y++)
		{
			if (level[Position{ x, y }] == '@')
			{
				playerPos = Position{ x, y };
				break;
			}
		}
	}
	if (playerPos == Position{ -1, -1 }) minDist = 0;
	vector<Position> safePoses;
	for (int x = 0; x < width; x++)
	{
		for (int y = 0; y < height; y++)
		{
			if (abs(x - playerPos.x) >= minDist && abs(y - playerPos.y) > minDist && level[Position{ x, y }] == '.')
			{
				safePoses.push_back(Position{ x, y });
			}
		}
	}
	auto targetPos = CHOOSE(safePoses);
	level[targetPos] = c;// 'g';
	return targetPos;
}

bool MapGenerator::FloodTest(map<Position, char> level)
{
	Position startPos{ -1, -1 };
	for (auto pair : level)
	{
		if (pair.second == '@')
		{
			startPos = pair.first;
			break;
		}
	}
	if (startPos == Position{ -1, -1 })
	{
		log("MapGenerator::FloodTest: No starting point found.");
		return false;
	}

	vector<Position> flood{ startPos };
	vector<Position> around{
		{ -1, -1 }, { -1, 0 }, { -1, 1 },
		{ 0, -1 }, { 0, 1 },
		{ 1, -1 }, { 1, 0 }, { 1, 1 }
	};

	while (true)
	{
		bool floodDone = true;
		for (auto pos : flood)
		{
			for (auto n : around)
			{
				Position newPos{ pos.x + n.x, pos.y + n.y };
				if (level[newPos] == 'X') return true;
				if (level[newPos] != '8' &&
					level[newPos] != '!' &&
					find(flood.begin(), flood.end(), newPos) != flood.end())
				{
					flood.push_back(newPos);
					floodDone = false;
				}
			}
		}
		if (floodDone) break;
	}
	return false;
}

vector<string> MapGenerator::Flatten(map<Position, char> level)
{
	Position maxPoint{ 0, 0 };
	for (auto pair : level)
	{
		maxPoint.x = max(maxPoint.x, pair.first.x);
		maxPoint.y = max(maxPoint.y, pair.first.y);
	}
	vector<string> result;
	for (int y = 0; y <= maxPoint.y; y++)
	{
		string line = "";
		for (int x = 0; x <= maxPoint.x; x++)
			line += nToString(level[Position{ x, y }]);
		result.push_back(line);
	}
	result.push_back("ENDGRID");
	return result;
}