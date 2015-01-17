#ifndef __SNAKELAYER_H__
#define __SNAKELAYER_H__

#include "helpers.h"
#include "LineNode.h"

namespace Snake
{
	class SnakeLayer : public Node
	{
	public:
		Label* lblScore;
		Label* lblInstruction;
		bool instructionVisible;
		Node* lineGroup;
		Node* bonusGroup;
		Node* itemGroup;
		Rect bounds;

		float maximumLength;
		int itemsPicked;
		int score;
		int bonus;
		Point direction;
		float speed;

		CREATE_FUNC(SnakeLayer);
		bool init();
		bool move(Directions);

		void update(float) override;
		void itemPicked();
		void bonusPicked();
		void considerSpawningBonus(float = 0);
		void removeBonus(float = 0);
		void spawnItem();
		void spawnBonus();
		void truncate();
		void drag(float dt);
		bool testDie();
		void testPick();
		void gameOver();
	};
}

#endif
