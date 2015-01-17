#ifndef __MAPGENERATOR_H__
#define __MAPGENERATOR_H__

#include "helpers.h"
#include "PlatformerLayer.h"
#include <map>
#include <string>

namespace Platformer
{
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
		static Position AddItem(char c, map<Position, char>&, int minDist = 0);
		static bool FloodTest(map<Position, char>);
		static vector<string> Flatten(map<Position, char>);
	};

	const MapGenerator::MapMeta MAPMETA_Platformer = {
		SCREENXCOUNT, //width
		SCREENYCOUNT, //height
		{ //codes (code, min, max)
			{ '@', 1, 1 },
			{ 's', 3, 10 },
			{ '8', 5, 20 },
			{ '0', 1, 10 },
			{ '!', 2, 10 },
			{ '+', 2, 10 },
			{ 't', 1, 1 },
			{ 'd', 1, 1 },
			{ 'T', 1, 1 },
			{ 'D', 1, 1 }
		},
		0
	};
}
#endif
