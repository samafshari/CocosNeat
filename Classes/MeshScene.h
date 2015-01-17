#ifndef __MeshScene_H
#define __MeshScene_H

#include "cocos2d.h"
#include "helpers.h"
#include "LineNode.h"
#include "Vertex.h"
#include "Graph.h"

#define GAME_TAG 1000
#define CURRENTGRAPH_TAG 1001
#define NEXTGRAPH_TAG 1002
#define PREVGRAPH_TAG 1003

USING_NS_CC;
class MeshScene : public Layer
{
protected:
	bool over;
public:
	vector<LineNode*> lines;
	LineNode* currentLine;
	Layer* prevGraph;
	Layer* currentGraph;
	Layer* nextGraph;
	Layer* game;

	VertexNode *lineBeginStuck, *lineEndStuck;
	bool swipeNextLevel;

	Label* timerLayer;
	Label* winsLayer;
	Label* winStatsLayer;
	int time;
	int level;
	int wins;
	bool brokeHighScore;
	int nodeCount;
	float defectChance;

    virtual bool init();  
    static cocos2d::Scene* scene();
	CREATE_FUNC(MeshScene);
	Menu* pMenu;

	void btnBackPressed(Ref*);
	void btnSoundPressed(Ref*);
	void btnHelpPressed(Ref*);
	void btnNextLevel(Ref*);

	void createStats();

	bool onTouchBegan(Touch*, Event*) override;
	void onTouchMoved(Touch*, Event*) override;
	void onTouchEnded(Touch*, Event*) override;

	void onTouchMoved_connect(Touch*);

	void onKeyReleased(EventKeyboard::KeyCode keyCode, Event* event) override;

	void createGame();
	void advanceLevel(float = 0);
	bool allowStick(VertexNode*);
	void applyPair();
	void checkWin();
	void winGame();
	void timerTick(float = 0);
	void loseGame(bool backPressed = false);
	void flashText(string text);
};

#endif // __MeshScene_H
