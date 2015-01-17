#ifndef __CONTRASTSCENE_H__
#define __CONTRASTSCENE_H__

#include "helpers.h"

namespace Contrast
{
	class ContrastScene : public Layer
	{
	public:
		LayerColor* background;

		virtual bool init();
		static Scene* createScene();
		CREATE_FUNC(ContrastScene);

		void onKeyReleased(EventKeyboard::KeyCode keyCode, Event* event) override;
		void onAcceleration(Acceleration* accel, Event*) override;
		void update(float) override;

		void showGameOver(Ref* = nullptr);
        void btnPausePressed(Ref* = nullptr);
        
		static int LastScore;
		static int HighScore;
		static int TotalScore;

		static void LoadSettings();
		static void SaveSettings();
        
		//////////////////GAME/////////////////
		int maxsize;
		float distance;
		float time;
		float bonustime;
		int bonusmultiplier;
		int wins;

		float rotationDirection;

		const int INIT_SIZE = 1;
		const int MAX_SIZE = 2;
		const float INIT_DISTANCE = 0.5f;
		const float MIN_DISTANCE = 0.04f;
		const int MAX_TIME = 30;
		const float CONTENT_WIDTH_P = 0.85f;
		const float CONTENT_GAP_P = 0.01f;
		const float MAX_BONUS_TIME = 3.99f;
		const int MAX_MULTIPLIER = 5;
		const int MAX_LIVES = 3;

		vector<function<vector<Point>()>> layouts;
		vector<function<Vec2(vector<Point>)>> gameModes;

		Node* previous;
		Node* current;
		Node* shader;

		Label* timerLayer;
		Label* winsLayer;
		
		Node* bonusLayer;

		void skip(Ref* = nullptr);
		void nextBoard();
		void win(Ref* = nullptr);
		void secret(Ref* = nullptr);
		void dummy(Ref* = nullptr);

		void createStats();
		void timerTick(float = 0);
		void bonusTick(float = 0);

		Node* pauseCover;
		void resumeSchedules();

		string block_sprite;
		float blockSize;

        Node* createButton(Color3B color, bool answer, string text="");
		
		vector<Point> createSquare();
        vector<Point> createBar();
        vector<Point> createPlus();

        vector<Color3B> colors;
        vector<string> labels;
        
        bool onTouchBegan(Touch*, Event*);
        
		Vec2 createColor(vector<Point>);
        Vec2 createColorWord(vector<Point>, bool opposite);
		Vec2 createColorWordNormal(vector<Point>);
        Vec2 createColorWordOpposite(vector<Point>);

		Vec2 createColorSpell(vector<Point>, bool opposite);
		Vec2 createColorSpellNormal(vector<Point>);
		Vec2 createColorSpellOpposite(vector<Point>);

		void enableRotation(Vec2);

		int lives;
		Node* livesLayer;
		void updateLivesUI();
	};
    
    class IntermissionScene : public Layer
    {
		Scene* target;

        virtual bool init();
        static Scene* createScene(Scene* _target);
		CREATE_FUNC(IntermissionScene);
        
        bool locked;
        void unlock(float=0);
		bool touched(Touch*, Event*);
    };
}

#endif
