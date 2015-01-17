#ifndef __GRAPH_H__
#define __GRAPH_H__

#include <vector>
#include <map>
#include <string>
#include "cocos2d.h"

USING_NS_CC;
using namespace std;

class Graph
{
public:
	static vector<int> GenerateNodes(int count, bool allowDefective = false);
	static Layer* GenerateUI(vector<int> nodes, float radius, float rdeltaX=0, float rdeltaY=0);
};

#endif
