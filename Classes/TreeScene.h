#ifndef __TREESCENE_H
#define __TREESCENE_H

#include "helpers.h"
#include "ExtraGameOverScene.h"

#define TREECOUNT		10
#define MANVELOCITY		0.6f
#define TREEVELOCITY	0.4f
#define TREECHANCE		0.08f
#define TREETIME		0.8f

#define ILLEGAL_POSITION Point(-1000,1000)

const vector<string> treeNames = { "tree0", "tree0", "tree0", "tree0", 
									"tree1", "tree1", "tree1" };

enum class TreeGameStates
{
	Menu,
	Initializing,
	Ready,
	Done
};

class TreeScene : public Layer
{
public:
    virtual bool init();  
    static cocos2d::Scene* scene();
	CREATE_FUNC(TreeScene);

	TreeGameStates state;

	int score;
	float treeTime;
	bool highScoreBeaten;
	inline string getTree() { return treeNames[rand() % treeNames.size()]; }
	Sprite* man;
	Layer* treeLayer;
	Label* lblScore;
	Label* lblHighScore;

	Directions manDirection;

	void die();
	void resetGame();
	void startGame(float=0);
	void readyGame(float=0);
	void resetTree(Sprite*);
	void update(float) override;
	void giveScore(int points);
	void onKeyReleased(EventKeyboard::KeyCode keyCode, Event* event) override;
	void move(Directions dir);

	void menuCloseCallback(Ref*);
	void btnExitPressed(Ref*);

	float treeVelocity;
	void onAcceleration(Acceleration* accel, Event*) override;
};

#endif
