#ifndef __GAMECENTER_H__
#define __GAMECENTER_H__

#include <string>
#include <functional>
#include <map>

#include "cocos2d.h"
using namespace std;
USING_NS_CC;

#if IS_IOS
#include "GKHWrapperCpp.h"
#include "GKHDelegate.h"
#include "GKCppAlternatives.h"
class GameKitDelegate : public GKHDelegate
{
public:
    // Players
    virtual void onLocalPlayerAuthenticationChanged() override;
    virtual void onFriendsListRecieved(std::vector<std::string> friends);
    virtual void onPlayerInfoReceived(std::vector<GKPlayerCpp> playerInfo);
    
    // Achievements
    //virtual void onAchievementsLoaded(std::map<std::string, GKAchievementCpp> achievements);
    
    // Game Centre Views
    //virtual void onLeaderboardViewDismissed();
    //virtual void onAchievementsViewDismissed();
    
    void onScoresReceived(vector<GKScoreCpp>) override;
};
#endif

struct ScorePair
{
	std::string Name;
	int Score;
	int TotalScore;
	string InstallId;
	const bool operator==(const ScorePair b) const;
};

#if IS_IOS
#define __GAMECENTER_H_GAMEKIT static GameKitDelegate GameKit
#else
#define __GAMECENTER_H_GAMEKIT 
#endif

#define GAMECENTER_HEADER \
	__GAMECENTER_H_GAMEKIT; \
	static std::map<std::string, std::string> PlayerNames; \
	static int GlobalRanking; \
	static int GlobalTotalScore; \
	static int GlobalLevelupDelta; \
	static std::string DefaultLeaderBoard; \
	static std::map<std::string, std::vector<ScorePair>> LeaderBoards; \
	static std::vector<ScorePair> GetLeaderBoard(std::string); \
	static std::vector<ScorePair> GetLeaderBoard(); \
	static void InitGameCenter(Games); \
	static void FetchGameCenterData(Games); \
	static void GetPlayersInfo(std::vector<std::string>); \
	static std::string GetPlayerName(std::string); \
	static std::function<void(string)> GameCenterDataReceived; \
	static int GetHighScore(); \
    static void ReportAchievement(string name, float percentComplete=100, bool showCompletion=true); \
	static void ReportScore(Games game, int score);\
	static void ReportScore(string category, int score);

#define ACHIEVEMENT_PROPLAYER               "grp.AchievementProPlayer" //Buy pro version
#define ACHIEVEMENT_UNLOCKED_ALL            "grp.AchievementUnlockedAll" //Unlock all games (run all at least once)
#define ACHIEVEMENT_UNLOCKED_MESH           "grp.AchievementUnlockedMesh" //run mesh once
#define ACHIEVEMENT_UNLOCKED_NATURALS       "grp.AchievementUnlockedNaturals" //run naturals once
#define ACHIEVEMENT_UNLOCKED_SNAKE          "grp.AchievementUnlockedSnake" //run snake once
#define ACHIEVEMENT_UNLOCKED_RAID           "grp.AchievementUnlockedRaid" //run raid once
#define ACHIEVEMENT_UNLOCKED_TREE           "grp.AchievementUnlockedTree" //run tree once
#define ACHIEVEMENT_UNLOCKED_FALL           "grp.AchievementUnlockedFall" //run fall once
#define ACHIEVEMENT_UNLOCKED_DONKEY			"grp.AchievementUnlockedDonkey"
#define ACHIEVEMENT_BUY_REGEN               "grp.AchievementBuyRegen" //buy fast regen
#define ACHIEVEMENT_BUY_GEM                 "grp.AchievementBuyGem" //Buy something (except regen) Merchant
#define ACHIEVEMENT_ADDICT_LOW              "grp.AchievementAddictLow" //Play game (overall) for 5 minutes
#define ACHIEVEMENT_ADDICT_MEDIUM           "grp.AchievementAddictMedium" //Play for 20 minutes
#define ACHIEVEMENT_ADDICT_HIGH             "grp.AchievementAddictHigh" //Play for 60 minutes
#define ACHIEVEMENT_LOSE_MESH               "grp.AchievementLoseMesh" //score 0 in a full session of mesh
#define ACHIEVEMENT_LOSE_RAID               "grp.AchievementLoseRaid" //score 0 in raid
#define ACHIEVEMENT_LOSE_DRONES             "grp.AchievementLoseDrones" //score 0 in drones
#define ACHIEVEMENT_SCORE_DRONES            "grp.AchievementScoreDrones" //score XX in drones
#define ACHIEVEMENT_SCORE_MESH              "grp.AchievementScoreMesh" //score 30 in mesh (Mathematician)
#define ACHIEVEMENT_SCORE_SNAKE             "grp.AchievementScoreSnake" //score 50 in snake
#define ACHIEVEMENT_SCORE_RAID              "grp.AchievementScoreRaid" //get a 1up in raid
#define ACHIEVEMENT_SCORE_TREE              "grp.AchievementScoreTree" //score 500 in tree
#define ACHIEVEMENT_SCORE_FALL              "grp.AchievementScoreFall" //score/survive 2 mins in fall (Abyss)
#define ACHIEVEMENT_RAID_ROOKIE             "grp.AchievementRaidRookie" //explode the first bridge
#define ACHIEVEMENT_RAID_CAPTAIN            "grp.AchievementRaidCaptain" //get to level 4
#define ACHIEVEMENT_RAID_COLONEL            "grp.AchievementRaidColonel" //get to level 8
#define ACHIEVEMENT_RAID_GENERAL            "grp.AchievementRaidGeneral" //get to level 16
#define ACHIEVEMENT_DRONES_KILLER           "grp.AchievementDronesKiller" //kill 100 drones
#define ACHIEVEMENT_REGEN_SESSION           "grp.AchievementRegenSession" //regenerate a full quota of a session
#define ACHIEVEMENT_GEM_REGEN               "grp.AchievementGemRegen" //regenerate 50 gems
#define ACHIEVEMENT_GEM_MINE                "grp.AchievementGemMine" //mine 10 gems in a session
#define ACHIEVEMENT_GEM_RICH                "grp.AchievementGemRich" //get rich by having 100 gems all at once
#define ACHIEVEMENT_GEM_SPENDER             "grp.AchievementGemSpender" //spend 100 gems
#define ACHIEVEMENT_GEM_EARNER              "grp.AchievementGemEarner" //earn overall 100 gems
#define ACHIEVEMENT_SNAKE_BONUS				"grp.AchievementSnakeBonus" //get 10 lollis

#define ACHIEVEMENT_EPISODE_1				"grp.AchievementGoDEpisode1" //finish episode 1
#define ACHIEVEMENT_EPISODE_2				"grp.AchievementGoDEpisode2" //finish episode 2
#define ACHIEVEMENT_EPISODE_3				"grp.AchievementGoDEpisode3" //finish episode 3
#define ACHIEVEMENT_FINISH_GAME				"grp.AchievementGoDFinishGame" //finish all episodes
#define ACHIEVEMENT_LOSE_EGGPLANT			"grp.AchievementLoseEggplant"

#define LEADERBOARD_DRONESARCADE			"grp.DronesArcadeScoreLeaderboard"
#define LEADERBOARD_EGGPLANT				"grp.EggplantScoreLeaderboard"
#define LEADERBOARD_FALL					"grp.FallScoreLeaderboard"
#define LEADERBOARD_RICH					"grp.RichLeaderboard"

#if defined(MINI_GAME)
#define LEADERBOARD_MESH					"grp.MeshScoreLeaderboard"
#else
#define LEADERBOARD_MESH					"grp.MeshScoreLeaderboard1"
#endif

#define LEADERBOARD_RAID					"grp.RaidScoreLeaderboard"
#define LEADERBOARD_RAID_LEVEL              "grp.RaidLevelLeaderboard"
#define LEADERBOARD_SNAKE					"grp.SnakeScoreLeaderboard"
#define LEADERBOARD_TREE					"grp.TreeScoreLeaderboard"
#define LEADERBOARD_DONKEY					"grp.DonkeyScoreLeaderboard"
#define LEADERBOARD_MINMOVES				"grp.GoDMinMovesLeaderboard"
#define LEADERBOARD_MINTIME					"grp.GoDMinTimeLeaderboard"
#define LEADERBOARD_MAXKILLS				"grp.GoDMaxKillsLeaderboard"

#define LEADERBOARD_MELODIE                 "grp.MelodieScoreLeaderboard"
#define LEADERBOARD_BLOB                    "grp.BlobScoreLeaderboard"
#define LEADERBOARD_MATRYOSHKA				"grp.MatryoshkaScoreLeaderboard"
#define LEADERBOARD_CONTRAST				"grp.ContrastScoreLeaderboard2"
#define LEADERBOARD_BOUNCYBIRD				"grp.BouncyBirdScoreLeaderboard"

#endif