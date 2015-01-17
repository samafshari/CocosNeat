#include "Graph.h"
#include "helpers.h"
#include "Vertex.h"
#include <random>

vector<int> Graph::GenerateNodes(int count, bool allowDefective)
{
	//Algorithm:
	//1. Generate a random graph with no loops (?)
	//1a. For each edge, make a connection with another edge
	//1b. At most we have N-1 edges otherwise the graph is not valid (?)
	//2. Label edges as even numbers
	//3. Divide numbers between connecting nodes
	
	//Defective doesn't work for 2
	if (count == 2) allowDefective = false;

	//create graph
	vector<int> edges;
	do
	{
		//Connect edges
		bool lastNodeConnected = false;
		for (int i = 0; i < count - 1; i++)
		{
			do
			{
				int endPoint = rand() % count;
				if (endPoint == i) continue;
				if (endPoint < i && edges[endPoint] == i) continue;
				edges.push_back(endPoint);
				if (endPoint == count - 1) lastNodeConnected = true;
				break;
			} while (true);
		}

		if (allowDefective) //connect the Nth node
		{
			int i = count - 1;
			do
			{
				int endPoint = rand() % count;
				if (endPoint == i) continue;
				if (endPoint < i && edges[endPoint] == i) continue;
				edges.push_back(endPoint);
				lastNodeConnected = true;
				break;
			} while (true);
		}

		if (!lastNodeConnected && !allowDefective)
		{
			//invalid graph because last node does not have any connections
			edges.clear();
			continue;
		}
		break;
	} while (true);

	//Label edges
	vector<int> edgeLabels;
	for (int i = 0; i < count - 1; i++)
		edgeLabels.push_back(2 * (rand() % 3 + 1));
	if (allowDefective) edgeLabels.push_back(2 * (rand() % 3 + 1));

	//Label nodes
	vector<int> result;
	for (int n = 0; n < count; n++)
	{
		int sum = (n < count - 1 ? edgeLabels[n] : 0);
		if (allowDefective) sum = edgeLabels[n];

		for (int e = 0; e < count - 1; e++)
		{
			if (n == e) continue;
			if (edges[e] == n) sum += edgeLabels[e];
		}
		if (allowDefective && n != count - 1 && edges[count - 1] == n) sum += edgeLabels[count - 1];
		result.push_back(sum / 2);
	}

	//shuffle nodes
	random_shuffle(result.begin(), result.end());

	//additional step, if all the numbers are divisible by the smallest,
	//TODO

	//log
	string s = "";
	for (auto node : result) s += nToString(node) + ",";
	log("Nodes:" + s);

	return result;
}

Layer* Graph::GenerateUI(vector<int> nodes, float radius, float rdeltaX, float rdeltaY)
{
	auto layer = Layer::create();
	int count = static_cast<int>(nodes.size());
	
	float initAlpha = rand() % 4 * 90; // 0; // RANDF * 360.0f;
	if (count == 3) initAlpha = rand() % 2 == 0 ? 90 : 270;
	if (count == 4) initAlpha = rand() % 2 == 0 ? 0 : 45;

	float rx = radius + RANDF * rdeltaX;
	float ry = radius + RANDF * rdeltaY;

	float delta = 360.0f / static_cast<float>(count);
	for (int i = 0; i < count; i++)
	{
		VertexNode* v = VertexNode::create(nodes[i]);
		v->setTag(i);
		float alpha = fmodf(initAlpha + delta * static_cast<float>(i), 360.0f);
		alpha = CC_DEGREES_TO_RADIANS(alpha);
		v->setPositionX(rx * cos(alpha));
		v->setPositionY(ry * sin(alpha));
		layer->addChild(v);
	}
	return layer;
}