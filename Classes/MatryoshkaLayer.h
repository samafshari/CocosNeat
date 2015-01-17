#ifndef __MATRYOSHKALAYER_H__
#define __MATRYOSHKALAYER_H__

#include "helpers.h"

namespace Matryoshka
{
	const string FRAME_PREFIX = "matr";
	inline string GetFrameName(int i) { return FRAME_PREFIX + nToString(i); }

	class MatryoshkaNode : public Node
	{
	public:
		Sprite* sprite;
		int number;

		CREATE_FUNC(MatryoshkaNode);
		virtual bool init();
		void upgrade(int);
		void poof();
	};

	class MatryoshkaLayer : public cocos2d::Layer
	{
	public:
		enum class GameStates
		{
			Idle,
			Playing,
			Paused,
			Done
		};
		
		map<Position, MatryoshkaNode*> board;
		Node* boardParent;

		int score;
		GameStates state;
		Label* lblScore;

		CREATE_FUNC(MatryoshkaLayer);
		virtual bool init();
		void gameOver();

		void move(Directions);
		Position move(Position, Directions);
		void move(Position dest, Position source);
		void spawn();
		void activate(Position);
		void erase(Position);
		void print();
		Vec2 calculatePosition(MatryoshkaNode*, Position);
		void reposition(MatryoshkaNode*, Position);
		bool checkGameOver();
	};
}

#endif
