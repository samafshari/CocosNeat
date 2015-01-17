#ifndef __MELODIELAYER_H__
#define __MELODIELAYER_H__

#include "helpers.h"

namespace Melodie
{
	const float FLASH_DURATION = 0.5f;
	const float PLAYBACK_PERIOD = 0.7f;
	const int BUTTONS_COUNT = 10;
	const int MAX_LIVES = 3;
	const int TRY_TIME = 10;
	enum class GameStates
	{
		Playback,
		Input,
		Idle
	};

	class MelodieLayer : public Node
	{
	public:
		int score;
		int lives;
		map<int, Node*> lit;
		vector<int> seq;
		int cursor;
		GameStates state;
		Label* scoreLabel, *stateLabel;
		Node* livesLayer;
		Sprite* circle;
		Label* bigText;

		CREATE_FUNC(MelodieLayer);
		bool init();

		void flashBigText(string, Color3B);
		void begin(float);
		void flash(int);
		void play();
		void play(float);
		void correct();
		void wrong();
		void makeNextMove();
		void giveScore(int);
		bool onTouchBegan(Touch*, Event*);
		void gameOver(float = 0);
		void updateLivesUI();
	};
}

#endif
