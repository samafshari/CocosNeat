#include "DronesScene.h"
#include "helpers.h"
#include "DronesEntity.h"
#include "MainMenuScene.h"
#include "PreGameScene.h"
#include "EpilogueScene.h"
#include "ChooserScene.h"
#include "HelpScene.h"
#include "ExtraGameOverScene.h"
#include "EpisodeChooserScene.h"
#include "Ooze.h"
#include "Widgets.h"
#include "BackgroundWidget.h"

using namespace Drones;

static int losses = 0;

void DronesScene::btnNextLevel(Ref*)
{
	cleanUpGame();
	GameSettings::CurrentLevelId++;
	Director::getInstance()->replaceScene
		(TransitionFlipX::create(1.0f, DronesScene::scene()));
}

void DronesScene::btnPrevLevel(Ref*)
{
	cleanUpGame();
	GameSettings::CurrentLevelId--;
	Director::getInstance()->replaceScene
		(TransitionFlipX::create(1.0f, DronesScene::scene()));
}

void DronesScene::btnChooseLevel(Ref*)
{
	cleanUpGame();
	Director::getInstance()->replaceScene(TransitionFadeBL::create(1.0f, ChooserScene::scene(
		12 * (GameSettings::CurrentLevelId / 12))));
}

bool DronesScene::move(Directions dir, bool doAlignCamera)
{
	if (!game) return false;
	if (game->state != GameStates::Ready) return false;
	
	bool result = false;
	if ((result = game->move(game->player, dir)))
	{
        if (!moved)
        {
            gettimeofday(&moveBeginTime, NULL);
            if (gameType == GameTypes::Level)
            {
                schedule(schedule_selector(DronesScene::updateGameStats), 1.0);
				//for (auto n : getChildByTag(NEWSTATSTAG)->getChildren()) n->setColor(Color3B::BLACK);
                getChildByTag(OLDSTATSTAG)->runAction(MoveTo::create(0.1f, getChildByTag(NEWSTATSTAG)->getPosition()));
                getChildByTag(NEWSTATSTAG)->runAction(MoveTo::create(0.2f, Point(0, 0)));
            }
            else if (gameType == GameTypes::Random)
            {
                GameSettings::LastTime--;
                schedule(schedule_selector(DronesScene::updateRandomTimer), 1.0f);
			}
			moved = true;
        }
		GameSettings::TotalMoves++;
		GameSettings::LastMoves++;
		moveHistory.push_back(dir);
        //PlayEffect("sound30.mp3");
		if (gameType == GameTypes::Level)
			updateGameStats();
		else if (gameType == GameTypes::Random || gameType == GameTypes::Eggplant)
			updateRandomStats();

	}
	if (doAlignCamera && !game->forceNoScroll) alignCamera();
	
	return result;
}

void DronesScene::btnBackPressed(Ref*)
{
	if (gameType == GameTypes::Random || gameType == GameTypes::Eggplant)
		btnSurrenderPressed(this);
	else if (gameType == GameTypes::Level)
    {
        AdsEnabled = true;
        ShowFullscreenAd();
		Director::getInstance()->replaceScene(TransitionFade::create(1.0f, ChooserScene::scene()));
    }
}

void DronesScene::btnRefreshPressed(Ref*)
{
	cleanUpGame();
	Director::getInstance()->replaceScene(TransitionFade::create(1.0f, DronesScene::scene()));
    game->removeAllChildrenWithCleanup(true);
	removeChild(game);
}

void DronesScene::btnSurrenderPressed(Ref*)
{
	GameSettings::LastTime = 0;
    randomGameOver(true);
}

void DronesScene::cleanUpGame()
{
    for (auto input : inputs)
	{
		input->kill();
		input->unscheduleAllSelectors();
        input->unscheduleUpdate();
        removeChild(input);
	}
    game->state = GameStates::Initializing;
    game->unscheduleAllSelectors();
    Entity::Game = nullptr;
    dead = true;
}

void DronesScene::advanceLevel(float dt)
{
    game->state = GameStates::Initializing;
    game->unscheduleAllSelectors();
    Entity::Game = nullptr;
    dead = true;
	for (auto input : inputs)
	{
		input->kill();
	}

	if (over && dt == 0) return;
	over = true;

	if (gameType == GameTypes::Random)
	{
		if (!game->didWin)
			Director::getInstance()->replaceScene(TransitionFade::create(1.0f, DronesScene::randomScene()));
		else
		{
			GameSettings::LastWins++;
			GameSettings::DronesLastScore += 5;
            GameSettings::LastTime += 10;
            //ooze out moves
            string moves = "";
            string levelmap = "";
            
            for (auto move : moveHistory) moves += nToString((int)move) + ",";
            for (auto line : levelData) levelmap += line + ",";
            Ooze::InstantSend({
                {"Event", "DronesArcadeWin"},
                {"LastWins", nToString(GameSettings::LastWins)},
                {"LastKills", nToString(GameSettings::LastKills)},
                {"LastScore", nToString(GameSettings::DronesLastScore)},
                {"Moves", nToString(game->moves)},
                {"Kills", nToString(game->kills)},
                {"Sequence", moves},
                {"Map", levelmap}
                });
			Director::getInstance()->replaceScene(TransitionFlipX::create(1.0f, DronesScene::randomScene()));
		}
		return;
	}

	if (gameType == GameTypes::Eggplant)
	{
		if (game->didWin)
		{
			GameSettings::LastWins++;
			GameSettings::DronesLastScore += 5;
			GameSettings::LastTime += 10;
			Director::getInstance()->replaceScene(TransitionFlipX::create(1.0f, DronesScene::eggplantScene()));
		}
		else
			Director::getInstance()->replaceScene(TransitionFade::create(1.0f, DronesScene::eggplantScene()));
		return;
	}

	if (!game->didWin)
	{
		Director::getInstance()->replaceScene(TransitionFade::create(2.0f, DronesScene::scene()));
        losses++;
        if (losses % 3 == 0)
        {
            AdsEnabled = true;
            ShowFullscreenAd();
        }
	}
	else
	{
		saveGameStats();
		UserDefault::getInstance()->setIntegerForKey(("wone" + nToString(GameSettings::CurrentEpisode) + "l" + nToString(GameSettings::CurrentLevelId)).c_str(), TAG_WIN);
		if (GameSettings::CurrentLevelId == GameSettings::GetMaximumLevelId() - 1)
		{
			GameSettings::AdvanceLevel();
			Director::getInstance()->replaceScene(TransitionRotoZoom::create(2.0f, EpilogueScene::createScene()));
			return;
		}
		else
		{
			GameSettings::GiveGems(1);
			GameSettings::AdvanceLevel();
			Director::getInstance()->replaceScene(TransitionFlipX::create(1.0f, DronesScene::scene()));
		}
	}
}

void DronesScene::onKeyReleased(EventKeyboard::KeyCode keyCode, Event* event)
{
	if (keyCode == EventKeyboard::KeyCode::KEY_ESCAPE)
		btnBackPressed(nullptr);
}

void DronesScene::alignCamera()
{
	float pX = static_cast<float>(game->player->position.x) * game->blockSize;
	float pY = static_cast<float>(game->player->position.y) * game->blockSize;

	float lX = (DESIGN_X - pX) * 0.5f;
	float lY = (DESIGN_Y - pY) * 0.5f;

	auto currentPos = game->getPosition();

	if (abs(currentPos.x - lX) < THRESHOLD_X * game->blockSize) lX = currentPos.x;
	if (abs(currentPos.y - lY) < THRESHOLD_Y * game->blockSize) lY = currentPos.y;

	moveCamera(lX, lY);
}

static Action* _gamemoveToAction = nullptr;
void DronesScene::moveCamera(float lX, float lY, float duration)
{
	if (lX > 0) lX = 0;
	if (lY > 0) lY = 0;
	if (lX < -((float)(Position::width) - SCREENXCOUNT) * game->blockSize)
		lX = -((float)(Position::width) - SCREENXCOUNT) * game->blockSize;
	if (lY < -((float)(Position::height - 1) - SCREENYCOUNT) * game->blockSize)
		lY = -((float)(Position::height - 1) - SCREENYCOUNT) * game->blockSize;
	
	auto oldlX = lX, oldlY = lY;
	if (game->gameSize.width < DESIGN_X || game->forceNoScroll) 
		lX = (DESIGN_X - game->gameSize.width) * 0.5f;
	if (game->gameSize.height < DESIGN_Y || game->forceNoScroll)
		lY = (DESIGN_Y - game->gameSize.height) * 0.5f;
	
	if (oldlX != lX || oldlY != lY) scrollable = true;
	if (game->forceNoScroll) scrollable = false;

	if (_gamemoveToAction != nullptr)
	{
		game->stopAction(_gamemoveToAction);
		_gamemoveToAction = nullptr;
	}
	
	if ((game->gameSize.width > DESIGN_X || game->gameSize.height > DESIGN_Y))
	{
		_gamemoveToAction = MoveTo::create(0.4f, Point(lX, lY));
		game->runAction(_gamemoveToAction);
	}
	else
		game->setPosition(Point(lX, lY));
}

void DronesScene::btnSoundPressed(Ref*)
{
	GameSettings::MuteSounds = !GameSettings::MuteSounds;
	string sprite = GameSettings::MuteSounds ? "btnMute" : "btnSound";
	string psprite = sprite + "Dn";
	sprite += "Up";
	btnSound->setSelectedImage(MAKESPRITE(psprite.c_str()));
	btnSound->setNormalImage(MAKESPRITE(sprite.c_str()));
	PlayEffect("bottle_pop_2.wav");
	GameSettings::SaveSettings();
}

void DronesScene::btnHelpPressed(Ref*)
{
	Director::getInstance()->pushScene(TransitionFade::create(0.2f, HelpScene::scene()));
}

void DronesScene::btnSkipLevel(Ref*)
{
	GameSettings::LastTime -= 3;
	if (GameSettings::LastTime <= 0)
	{
		GameSettings::LastTime = 0;
		game->didWin = false;
		randomGameOver();
		return;
	}
	Director::getInstance()->replaceScene(TransitionSlideInR::create(1.0f, DronesScene::randomScene()));
}

void DronesScene::randomGameOver(bool backPressed)
{
	dead = true;
	Games game = Games::Drones;
	if (gameType == GameTypes::Random)
	{
		game = Games::DronesRandom;
		if (!backPressed && GameSettings::DronesLastScore == 0)
			GameSettings::ReportAchievement(ACHIEVEMENT_LOSE_DRONES);

		GameSettings::ReportScore(LEADERBOARD_DRONESARCADE, GameSettings::DronesLastScore);
		if (GameSettings::DronesLastScore >= 100)
			GameSettings::ReportAchievement(ACHIEVEMENT_SCORE_DRONES);
		GameSettings::DronesTotalScore += GameSettings::DronesLastScore;
	}
	else if (gameType == GameTypes::Eggplant)
	{
		game = Games::DronesEggplant;
		if (!backPressed && GameSettings::DronesLastScore == 0)
			GameSettings::ReportAchievement(ACHIEVEMENT_LOSE_EGGPLANT);
		GameSettings::ReportScore(LEADERBOARD_EGGPLANT, GameSettings::DronesLastScore);
		GameSettings::DronesTotalScore += GameSettings::DronesLastScore;
	}

	Node* background = Node::create();

	auto bgcolor = MAKESPRITE("bg");
	bgcolor->setPosition(CP(0.5f, 0.5f));
	bgcolor->setAnchorPoint(Point(0.5f, 0.5f));
	bgcolor->setColor(Color3B(20, 20, 80));
	background->addChild(bgcolor);
	bgcolor->setOpacity(0);
	bgcolor->runAction(
		Sequence::create(
		MoveBy::create(0.1f, Point::ZERO),
		FadeIn::create(0.5f),
		MoveBy::create(3.0f, Point::ZERO),
		NULL));

	background->addChild(BackgroundWidget::create("bg", Color3B(20, 20, 80)));
	background->addChild(Vignette::create());

	Director::getInstance()->replaceScene(TransitionFade::create(1.0f, 
		ExtraGameOverScene::createScene(game, background)));
}