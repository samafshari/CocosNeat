#ifndef __MAPGENERATOR_H__
#define __MAPGENERATOR_H__

#include "helpers.h"
#include <map>
#include <string>

class MapGenerator
{
public:
	struct CodeFrequency
	{
		char code;
		int minCount;
		int maxCount;
	};
	struct MapMeta
	{
		int width;
		int height;
		vector<CodeFrequency> codes;
		int seed;
	};
	static map<Position, char> Generate(MapMeta);
	static Position AddItem(char c, map<Position, char>&, int minDist, int width, int height);
	static bool FloodTest(map<Position, char>);
	static vector<string> Flatten(map<Position, char>);
};

extern const MapGenerator::MapMeta GAME_OF_DRONES_MAPMETA;
extern const MapGenerator::MapMeta EGGPLANT_MAPMETA;
#endif
