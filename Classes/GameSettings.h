#ifndef __GAMESETTINGS_H
#define __GAMESETTINGS_H

#define EMSG_DIED "died"
#define EMSG_MOVED "moved"

#define DEF_SCORE(g) static int g##LastScore, g##HighScore, g##TotalScore;

#include "helpers.h"
#include "GameCenter.h"
#include "Purchase.h"
#include "GameInfo.h"

USING_NS_CC;

extern Scene* GetMainScene();
extern const string DRONES_MENU_MUSIC;

#define RATE_POPUP_TIME (4 * 60)
struct EpisodeInfo
{
	string title;
	int price;
	string music;
};

struct GameInfo
{
	int price;
    std::string title;
	Color4B color;
    std::string logo;
    std::string description;
	int* pLastScore;
	int* pHighScore;
    int* pTotalScore;
	std::string leaderboardId;
	std::string pictureUrl;
	Color4B neatColor;
	bool isLight;
};

extern map<Games, GameInfo> GameInfos;
extern vector<EpisodeInfo> EpisodeInfos;
extern bool EngineRunning;

class GameSettings
{
private:
	static void LoadLevelsFromList(const std::string& listFile);
public:
	GAMECENTER_HEADER;
	static map<string, string> LocalizedStrings;
	static std::vector< std::vector<std::string> > Episodes;
	
	static bool MuteSounds;
	static void SetMuteMusic(bool);
	static void PlayMusic(string);
	static void StopMusic();
	static bool IsMusicMuted;
	static string CurrentTrack;

	static bool AutoController;
	static int CurrentEpisode;
	static int CurrentLevelId; //First level is 0
	static vector< int > UnlockedLevelIds;
	static set< int > UnlockedEpisodes;
	static set< int > FinishedEpisodes;
	
	DEF_SCORE(Eggplant);
	DEF_SCORE(Drones);
	DEF_SCORE(Mesh);
	DEF_SCORE(Snake);
	DEF_SCORE(Raid);
	DEF_SCORE(Tree);
	DEF_SCORE(Fall);
	DEF_SCORE(Donkey);

	static int TotalMoves;
	static int LastKills;
    static int TotalKills;
	static int LastTime;
	static int LastMoves;
	static int LastWins;
    static int TotalMenus;

	static int Gems, RegeneratedGems, TotalEarnedGems, TotalSpentGems, MinedGems;
    static int TotalSecondsPlayed;
    static bool FastRechargeBought;
    static bool MeshUnlocked;
    static bool NaturalsUnlocked;
    static bool SnakeUnlocked;
    static bool RaidUnlocked;
    static bool TreeUnlocked;
    static bool FallUnlocked;
	static bool DronesArcadeUnlocked;
	static bool DonkeyUnlocked;

    static int MeshPlayed;
    static int NaturalsPlayed;
    static int SnakePlayed;
    static int RaidPlayed;
    static int TreePlayed;
    static int FallPlayed;
    static int DronesArcadePlayed;
	static int EggplantPlayed;
	static int DonkeyPlayed;

	static int PlayerR, PlayerG, PlayerB;

	static inline int GetGems() { return Gems; }

	static void LoadSettings(void);
	static void SaveSettings(void);

	static std::string GetLevelFilename();
	static void AdvanceLevel();
	static int GetMaximumLevelId();
	static int GetUnlockedLevelId();
	
	static void TryGiveUnlockAll();
	static void GiveGems(int points);
	static bool SpendGems(int points);
	static void OozeGameStats();
	static const int ADDICT_TIME_LOW = 20 * 60;
	static const int ADDICT_TIME_MEDIUM = 60 * 60;
	static const int ADDICT_TIME_HIGH = 120 * 60;

	static void Set(std::string name, int);
	static void Set(std::string name, float);
	static void Set(std::string name, double);
	static void Set(std::string name, std::string);
	static void Set(std::string name, bool = true);
	static void Set(std::string name, FLOAT2);
	static void Set(std::string name, FLOAT3);
	static void Set(std::string name, FLOAT4);
	static void Set(std::string name, std::vector<std::string>);

	static int GetInt(std::string name, int = 0);
	static float GetFloat(std::string name, float = 0);
	static double GetDouble(std::string name, double = 0);
	static std::string GetStr(std::string name, std::string = "");
	static bool GetBool(std::string name, bool = false);
	static FLOAT2 GetFLOAT2(std::string name, FLOAT2 = FLOAT2::ZERO);
	static FLOAT3 GetFLOAT3(std::string name, FLOAT3 = FLOAT3::ZERO);
	static FLOAT4 GetFLOAT4(std::string name, FLOAT4 = FLOAT4::ZERO);
	static std::vector<std::string> GetVector(std::string name, std::vector<std::string> = {});

	static inline void LoadInt(std::string name, int& x)
	{
		x = GetInt(name, x);
	}
	static inline void LoadFloat(std::string name, float& x)
	{
		x = GetFloat(name, x);
	}
	static inline void LoadDouble(std::string name, double& x)
	{
		x = GetDouble(name, x);
	}
	static inline void LoadStr(std::string name, std::string& x)
	{
		x = GetStr(name, x);
	}
	static inline void LoadBool(std::string name, bool& x)
	{
		x = GetBool(name, x);
	}
	static inline void LoadFLOAT2(std::string name, FLOAT2& x)
	{
		x = GetFLOAT2(name, x);
	}
	static inline void LoadFLOAT3(std::string name, FLOAT3& x)
	{
		x = GetFLOAT3(name, x);
	}
	static inline void LoadFLOAT4(std::string name, FLOAT4& x)
	{
		x = GetFLOAT4(name, x);
	}
	static inline void LoadVector(std::string name, std::vector<std::string>& x)
	{
		x = GetVector(name, x);
	}

	static void Flush();
};

#endif