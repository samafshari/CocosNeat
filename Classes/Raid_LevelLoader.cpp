#include "Raid.h"
#include "ExtraGameOverScene.h"
#include "Ooze.h"

using namespace Raid;

void RaidLayer::loadLevel(int _levelId)
{
	//log("Filename: " + GameSettings::GetLevelFilename(_levelId));
	//auto data = ReadLinesFromFile(GameSettings::GetLevelFilename(_levelId));
	vector<Color4B> colors{
		Color4B(100, 100, 100, 255),
		Color4B(20, 140, 20, 255),
		Color4B(20, 140, 20, 255),
		Color4B(200, 200, 100, 255),
		Color4B(10,10,10,255)
	};
	auto data = LevelGenerator::GenerateLevel(_levelId);
	srand(_levelId);
	if (_levelId < 0) srand(RaidScene::Level);
	loadLevel(data, CHOOSE(colors));
}

void RaidLayer::loadLevel(vector<string> data, Color4B color)
{
	Layer* level = Layer::create();

	int version = 1;
	const float bridgeW = 571;
	const float bridgeH = 100;

	vector<Point> innerWalls, outerWalls;
	float offset = 0;// FRAME_SIZE.height;
	if (levels->getChildrenCount() > 0)
	{
		auto lastChild = static_cast<LayerColor*>(*(levels->getChildren().rbegin()));
		offset = lastChild->getContentSize().height;
	}
	int line = 0;
	string versionId = data[line++];
	if (versionId == "~2")
	{
		versionId = data[line++];
		version = 2;
	}
	if (versionId != "~inner")
	{
		log("File is damaged. Begin not found.");

		assert(false);
		return;
	}

	//read inner walls
	string s;
	while ((s = data[line++]) != "~outer")
	{
		int cpos = 0;
		string xStr = ReadBlock(s, cpos, ',');
		string yStr = ReadBlock(s, cpos);
		Point v;
		v.x = nToFloat(xStr);
		v.y = fabs(nToFloat(yStr)) + offset;
		innerWalls.push_back(v);
	}

	//read outer walls
	while ((s = data[line++]) != "~entities")
	{
		int cpos = 0;
		string xStr = ReadBlock(s, cpos, ',');
		string yStr = ReadBlock(s, cpos);
		Point v;
		v.x = nToFloat(xStr);
		v.y = fabs(nToFloat(yStr)) + offset;
		if (version == 2) v.x = DESIGN_X * 0.5f - v.x;
		outerWalls.push_back(v);
	}
	outerWalls.push_back(Point(bridgeW, (*outerWalls.rbegin()).y + bridgeH));

	while (line < static_cast<int>(data.size()))
	{
		//read position
		s = data[line++];
		int cpos = 0;
		string xStr = ReadBlock(s, cpos, ',');
		string yStr = ReadBlock(s, cpos);
		Point v;
		v.x = nToFloat(xStr);
		if (version == 2) v.x = DESIGN_X * 0.5f - v.x;
		v.y = fabs(nToFloat(yStr)) + offset;

		//read flip
		s = data[line++];
		bool flip = nToBool(s);

		//read item
		s = data[line++];
		Items itype = (Items) nToInt(s);

		Entity* e = Entity::MakeEntity(itype, v, flip);
		if (e) level->addChild(e, 1);
	}

	//create rects
	for (size_t i = 1; i < outerWalls.size(); i++)
	{
		LayerColor* wallA = LayerColor::create(color,
			outerWalls[i - 1].x, outerWalls[i - 1].y - outerWalls[i].y);
		wallA->setPosition(Point(0, outerWalls[i].y));

		LayerColor* wallB = LayerColor::create(color,
			outerWalls[i - 1].x, outerWalls[i - 1].y - outerWalls[i].y);
		wallB->setPosition(Point(DESIGN_X - outerWalls[i - 1].x, outerWalls[i].y));

		level->addChild(wallA);
		level->addChild(wallB);
		/*
		LabelTTF* l = LabelTTF::create(nToString((int) i), "Arial", 24.0f);
		l->setFontFillColor(Color3B::BLACK);
		l->setColor(Color3B::BLACK);
		l->setPosition(Point(0.5f * DESIGN_X, outerWalls[i].y));
		l->setAnchorPoint(Point(0.5f, 0));
		level->addChild(l);
		*/
	}
	float height = outerWalls[outerWalls.size() - 1].y;
	level->setContentSize(Size(DESIGN_X, height));

	for (size_t i = 1; i < innerWalls.size(); i++)
	{
		if (innerWalls[i - 1].x == 0) continue;
		LayerColor* wall = LayerColor::create(color,
			2.0f * innerWalls[i - 1].x, innerWalls[i - 1].y - innerWalls[i].y);
		wall->setPosition(DESIGN_X * 0.5f - innerWalls[i - 1].x, innerWalls[i].y);
		level->addChild(wall);
	}

	float bridgeX = LevelGenerator::BRIDGE_WIDTH_BLOCKS * LevelGenerator::RIBBON_WIDTH;
	auto roadLeft = MAKESPRITE("road");
	roadLeft->setAnchorPoint(Point(1, 1));
	roadLeft->setPositionX(DESIGN_X * 0.5f - bridgeX);
	roadLeft->setPositionY(height);
	roadLeft->setScaleY(0.5f);
	level->addChild(roadLeft);

	auto roadRight = MAKESPRITE("road");
	roadRight->setAnchorPoint(Point(0, 1));
	roadRight->setPositionX(DESIGN_X * 0.5f + bridgeX);
	roadRight->setPositionY(height);
	roadRight->setScaleY(0.5f);
	level->addChild(roadRight);

	levels->addChild(level);
}

void RaidLayer::removeInvisibleEntities()
{
    int nodesDeleted = 0;
	for (auto level : levels->getChildren())
	{
		vector<Node*> nodesToDelete;
		float offsetY = level->getPositionY() + levels->getPositionY() + gameLayer->getPositionY();
		for (auto node : level->getChildren())
		{
			if (node->getBoundingBox().getMaxY() + offsetY < 0)
				nodesToDelete.push_back(node);
		}
		for (auto node : nodesToDelete)
		{
            nodesDeleted++;
			level->removeChild(node);
		}
	}
    //log("Deleted " + nToString(nodesDeleted) + " nodes");
}

void RaidLayer::streamLevels()
{
	bool needsStreaming = false;
	RaidLayer* level = static_cast<RaidLayer*>(*(levels->getChildren().rbegin()));
	float offsetY = level->getPositionY() + levels->getPositionY() + gameLayer->getPositionY();
	if (level->getChildrenCount() == 0) needsStreaming = true;
	else
	{
		float y = level->getContentSize().height + offsetY;
		if (y < FRAME_SIZE.height) needsStreaming = true;
	}
	if (needsStreaming)
	{
//loadLevel(RaidScene::Level);
		loadLevel(RaidScene::Level + 1);
	}
}
