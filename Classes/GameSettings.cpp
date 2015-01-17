#include "helpers.h"
#include "GameSettings.h"
#include "Ooze.h"
#include "MelodieScene.h"
#include "MatryoshkaScene.h"
#include "ContrastScene.h"
#include "Blob.h"
#include "tinyxml2/tinyxml2.h"

#if defined(DEBUG) && !defined(UNLOCKED_ALL_LEVELS)
#define UNLOCK_ALL_LEVELS
#endif

#define SET_SCORE(g) \
	int GameSettings::g##LastScore = 0; \
	int GameSettings::g##HighScore = 0; \
	int GameSettings::g##TotalScore = 0;

#define LOAD_SCORE(g) \
	g##LastScore = d->getIntegerForKey(#g "LastScore", g##LastScore); \
	g##HighScore = d->getIntegerForKey(#g "HighScore", g##HighScore); \
	g##TotalScore = d->getIntegerForKey(#g "TotalScore", g##TotalScore);

#define SAVE_SCORE(g) \
	d->setIntegerForKey(#g "LastScore", g##LastScore); \
	d->setIntegerForKey(#g "HighScore", g##HighScore); \
	d->setIntegerForKey(#g "TotalScore", g##TotalScore);

using namespace std;

vector<vector<string>> GameSettings::Episodes;

map<string, string> GameSettings::LocalizedStrings;

bool GameSettings::MuteSounds = false;
bool GameSettings::IsMusicMuted = false;
string GameSettings::CurrentTrack = "";
bool GameSettings::AutoController = true;

int GameSettings::CurrentEpisode = 0;
int GameSettings::CurrentLevelId = 0; //First level is 0
vector<int> GameSettings::UnlockedLevelIds;
set<int> GameSettings::UnlockedEpisodes;
set<int> GameSettings::FinishedEpisodes;

SET_SCORE(Drones);
SET_SCORE(Mesh);
SET_SCORE(Snake);
SET_SCORE(Raid);
SET_SCORE(Tree);
SET_SCORE(Fall);
SET_SCORE(Donkey);
SET_SCORE(Eggplant);

int GameSettings::PlayerR = 240;
int GameSettings::PlayerG = 255;
int GameSettings::PlayerB = 210;

int GameSettings::TotalMoves = 0;
int GameSettings::LastKills = 0;
int GameSettings::TotalKills = 0;
int GameSettings::LastTime = 0;
int GameSettings::LastMoves = 0;
int GameSettings::LastWins = 0;
int GameSettings::TotalMenus = 0;

#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)
int GameSettings::Gems = 25;
#else
int GameSettings::Gems = 9999999;// 25;
#endif
int GameSettings::RegeneratedGems = 0;
int GameSettings::TotalEarnedGems = GameSettings::Gems;
int GameSettings::TotalSpentGems = 0;
int GameSettings::MinedGems = 0;

int GameSettings::TotalSecondsPlayed = 0;
bool GameSettings::FastRechargeBought = false;

//#if (CC_TARGET_PLATFORM != CC_PLATFORM_ANDROID)
//#ifndef PRO_VERSION
//static bool ___unlocked___ = false;
//#else
//static bool ___unlocked___ = true;
//#endif
//#else
//static bool ___unlocked___ = true;
//#endif

static bool ___unlocked___ = true;

bool GameSettings::DronesArcadeUnlocked = false;
bool GameSettings::MeshUnlocked = ___unlocked___;
bool GameSettings::NaturalsUnlocked = ___unlocked___;
bool GameSettings::SnakeUnlocked = ___unlocked___;
bool GameSettings::RaidUnlocked = ___unlocked___;
bool GameSettings::TreeUnlocked = ___unlocked___;
bool GameSettings::FallUnlocked = ___unlocked___;
bool GameSettings::DonkeyUnlocked = ___unlocked___;

int GameSettings::MeshPlayed = 0;
int GameSettings::NaturalsPlayed = 0;
int GameSettings::SnakePlayed = 0;
int GameSettings::RaidPlayed = 0;
int GameSettings::TreePlayed = 0;
int GameSettings::FallPlayed = 0;
int GameSettings::DronesArcadePlayed = 0;
int GameSettings::EggplantPlayed = 0;
int GameSettings::DonkeyPlayed = 0;

map<Games, GameInfo> GameInfos
{
	{ Games::Mesh, { 10, ("Mesh"), Color4B(0, 70, 140, 255), "meshbig",
	("DESC_MESH"),
	&GameSettings::MeshLastScore, 
	&GameSettings::MeshHighScore, 
	&GameSettings::MeshTotalScore,
	LEADERBOARD_MESH,
	"https://fbcdn-photos-a-a.akamaihd.net/hphotos-ak-xaf1/t39.2081-0/p128x128/10333103_1466839953571082_2067295910_n.png",
	Color4B(0, 70, 140, 255)
	} },

	{ Games::Mesh2, { 10, "Mesh2", Color4B(255, 255, 255, 255), "meshbig",
	("DESC_MESH"),
	&GameSettings::MeshLastScore,
	&GameSettings::MeshHighScore,
	&GameSettings::MeshTotalScore,
	LEADERBOARD_MESH,
	"https://fbcdn-photos-a-a.akamaihd.net/hphotos-ak-xaf1/t39.2081-0/p128x128/10333103_1466839953571082_2067295910_n.png",
	Color4B(127, 30, 0, 255)
	} },

	{ Games::Tree, { 10, ("TreeMassacre"), Color4B(50, 50, 50, 255), "treebig",
	("DESC_TREE"),
	&GameSettings::TreeLastScore, 
	&GameSettings::TreeHighScore, 
	&GameSettings::TreeTotalScore,
	LEADERBOARD_TREE,
	"",
	Color4B(0, 0, 0, 255)
	} },

	{ Games::Snake, { 10, ("Snake"), Color4B(100, 0, 0, 255), "snakebig",
	("DESC_SNAKE"),
	&GameSettings::SnakeLastScore, 
	&GameSettings::SnakeHighScore, 
	&GameSettings::SnakeTotalScore,
	LEADERBOARD_SNAKE,
	"https://fbcdn-photos-e-a.akamaihd.net/hphotos-ak-xap1/t39.2081-0/p128x128/10333103_1534452246774621_1416840795_n.png",
	Color4B(175, 0, 0, 255)
	} },

	{ Games::Raid, { 10, ("Raid"), Color4B(0, 100, 0, 255), "raidbig",
	("DESC_RAID"),
	&GameSettings::RaidLastScore, 
	&GameSettings::RaidHighScore, 
	&GameSettings::RaidTotalScore,
	LEADERBOARD_RAID,
	"https://fbcdn-photos-h-a.akamaihd.net/hphotos-ak-xfp1/t39.2081-0/p128x128/10574696_1517686358449858_1350433606_n.png",
	Color4B(0, 75, 0, 255)
	} },

	{ Games::Fall, { 10, "Fall", Color4B(100, 0, 100, 255), "fallbig",
	("DESC_FALL"),
	&GameSettings::FallLastScore, 
	&GameSettings::FallHighScore, 
	&GameSettings::FallTotalScore,
	LEADERBOARD_FALL
	} },
	
#ifdef BURGERMAN
#define DRONES_NAME "Burgerman"
#define DRONES_DESC "DESC_BURGERMAN"
#else
#define DRONES_NAME "GameOfDrones"
#define DRONES_DESC "DESC_DRONES"
#endif

	{ Games::Drones, { 10, DRONES_NAME, Color4B(100, 0, 100, 255), "gamelogo",
	DRONES_DESC,
	nullptr,
	nullptr,
	nullptr,
	""
	} },

	{ Games::DronesRandom, { 10, "DronesRandom", Color4B(100, 0, 100, 255), "gamelogo",
	"DESC_DRONESARCADE",
	&GameSettings::DronesLastScore,
	&GameSettings::DronesHighScore,
	&GameSettings::DronesTotalScore,
	LEADERBOARD_DRONESARCADE
	} },

	{ Games::DronesEggplant, { 10, "DronesEggplant", Color4B(100, 0, 100, 255), "gamelogo",
	"DESC_DRONESARCADE",
	&GameSettings::EggplantLastScore,
	&GameSettings::EggplantHighScore,
	&GameSettings::EggplantTotalScore,
	LEADERBOARD_DRONESARCADE
	} },

	{ Games::Donkey, { 10, "Donkey", Color4B(0, 100, 70, 255), "gamelogo",
	("DESC_DONKEY"),
	&GameSettings::DonkeyLastScore, 
	&GameSettings::DonkeyHighScore, 
	&GameSettings::DonkeyTotalScore,
	""
	} },

	{ Games::Melodie, { 10, "Melodie", Color4B(0, 0, 0, 255), "melodiebig",
	("DESC_MELODIE"),
	&Melodie::MelodieScene::LastScore, 
	&Melodie::MelodieScene::HighScore, 
	&Melodie::MelodieScene::TotalScore,
	LEADERBOARD_MELODIE,
	"https://fbcdn-photos-b-a.akamaihd.net/hphotos-ak-xfp1/t39.2081-0/p128x128/10574681_452158874923733_1418353235_n.png",
	Color4B(74, 0, 140, 255)
	} },

	{ Games::Blob, { 10, "Blob", Color4B(0, 0, 0, 255), "melodiebig",
	"DESC_BLOB",
	&Blob::BlobScene::LastScore, 
	&Blob::BlobScene::HighScore, 
	&Blob::BlobScene::TotalScore,
	LEADERBOARD_BLOB,
	"https://fbexternal-a.akamaihd.net/safe_image.php?d=AQDEOKS4w9ZqLA_E&w=128&h=128&url=https%3A%2F%2Ffbcdn-photos-b-a.akamaihd.net%2Fhphotos-ak-xpf1%2Ft39.2081-0%2F10173497_180142028823231_487298597_n.png&cfs=1"
	} },

	{ Games::Matryoshka, { 10, "Matryoshka", Color4B(175, 50, 0, 255), "",
	"DESC_MATRYOSHKA",
	&Matryoshka::MatryoshkaScene::LastScore, 
	&Matryoshka::MatryoshkaScene::HighScore,
	&Matryoshka::MatryoshkaScene::TotalScore,
	LEADERBOARD_MATRYOSHKA,
	"",
	Color4B(175, 50, 0, 255)
	} },

	{ Games::Bundle, { 10, "Bundle", Color4B(100, 0, 100, 255), "gamelogo",
	("DESC_BUNDLE"),
	nullptr, nullptr, nullptr,
	LEADERBOARD_RICH
	} },

	{ Games::Platformer, { 10, "Platformer", Color4B(100, 0, 100, 255), "gamelogo",
	("DESC_PLATFORMER"),
	nullptr, nullptr, nullptr,
	LEADERBOARD_RICH,
	"",
	Color4B(175,50,0,255)
	} },

	{ Games::Contrast, { 10, "Contrastic", Color4B(255,255,255,255), "contrastbig",
	"DESC_Contrast",
	&Contrast::ContrastScene::LastScore, 
	&Contrast::ContrastScene::HighScore, 
	&Contrast::ContrastScene::TotalScore,
	LEADERBOARD_CONTRAST,
	"https://fbcdn-photos-g-a.akamaihd.net/hphotos-ak-xpa1/t39.2081-0/p128x128/10541000_751574081576214_1080977024_n.png",
	Color4B(250, 250, 250, 255)
	} },

	{ Games::BouncyBird, { 10, "BouncyBird", Color4B(100, 255, 100, 255), "gamelogo",
	"DESC_BOUNCYBIRD",
	nullptr, nullptr, nullptr,
	LEADERBOARD_BOUNCYBIRD,
	"",
	Color4B(50, 100, 0, 255)
	} }
};

#if defined(S_GAME)
const Games APPLICATION = Games::S_GAME;
#else
const Games APPLICATION = Games::Bundle;
#endif

vector<EpisodeInfo> EpisodeInfos
{
	{ "Moon", 20, "Black Vortex.mp3" },
	{ "Mars", 30, "Hitman.mp3" },
	{ "Uranus", 40, "All This.mp3" }
};

const string DRONES_MENU_MUSIC = "The Complex.mp3";

bool EngineRunning = false;

void GameSettings::LoadLevelsFromList(const string& listFile)
{
	Episodes.push_back(ReadLinesFromFile(listFile));
}

void GameSettings::LoadSettings(void)
{
	Ooze::LoadSettings();
	if (APPLICATION == Games::Platformer)
	{
		LoadLevelsFromList("platformer1.lst");
	}
	else
	{
		LoadLevelsFromList("levels1.lst");
		LoadLevelsFromList("levels2.lst");
		LoadLevelsFromList("levels3.lst");
	}

	auto d = UserDefault::getInstance();
	int checker = d->getIntegerForKey("checker", 0);

	MuteSounds = d->getBoolForKey("MuteSounds", MuteSounds);
	IsMusicMuted = d->getBoolForKey("IsMusicMuted", IsMusicMuted);
	CurrentTrack = d->getStringForKey("CurrentTrack", CurrentTrack);

	AutoController = d->getBoolForKey("AutoController", AutoController);
	CurrentEpisode = d->getIntegerForKey("CurrentEpisode", CurrentEpisode);
	CurrentLevelId = d->getIntegerForKey("CurrentLevelId", CurrentLevelId);

	for (size_t i = 0; i < Episodes.size(); i++)
	{
		bool __defaultUnlock__ = false;
#if PRO_VERSION
		__defaultUnlock__ = true;
#endif
		if (d->getBoolForKey(("UnlockedEpisode" + nToString((int) i)).c_str(), 
			__defaultUnlock__))
			UnlockedEpisodes.insert((int) i);
		UnlockedLevelIds.push_back(0);
	}

	//first episode is always unlocked
	//UnlockedEpisodes.insert(0);

	for (size_t i = 0; i < Episodes.size(); i++)
	{
		if (d->getBoolForKey(("FinishedEpisode" + nToString((int) i)).c_str(),
			false))	FinishedEpisodes.insert((int) i);
	}

#if (CC_TARGET_PLATFORM == CC_PLATFORM_WIN32) || defined(UNLOCK_ALL_LEVELS)
	for (size_t i = 0; i < Episodes.size(); i++)
		UnlockedLevelIds[i] = Episodes[i].size();
#else
	if (checker == 667)
	{
		for (size_t i = 0; i < Episodes.size(); i++)
		{
			UnlockedLevelIds[i] =
				d->getIntegerForKey(("UnlockedLevelId" + nToString((int) i)).c_str(), UnlockedLevelIds[i]);
		}
	}
#endif

	LOAD_SCORE(Drones);
	LOAD_SCORE(Eggplant);
	LOAD_SCORE(Mesh);
	LOAD_SCORE(Snake);
	LOAD_SCORE(Raid);
	LOAD_SCORE(Tree);
	LOAD_SCORE(Fall);
	LOAD_SCORE(Donkey);

	TotalMoves = d->getIntegerForKey("TotalMoves", TotalMoves);
	LastKills = d->getIntegerForKey("LastKills", LastKills);
    TotalKills = d->getIntegerForKey("TotalKills", TotalKills);
	LastTime = d->getIntegerForKey("LastTime", LastTime);
	LastMoves = d->getIntegerForKey("LastMoves", LastMoves);
	LastWins = d->getIntegerForKey("LastWins", LastWins);
    TotalMenus = d->getIntegerForKey("TotalMenus", TotalMenus);
    
	Gems = d->getIntegerForKey("Gems", Gems);
    RegeneratedGems = d->getIntegerForKey("RegeneratedGems", RegeneratedGems);
    TotalEarnedGems = d->getIntegerForKey("TotalEarnedGems", TotalEarnedGems);
    TotalSpentGems = d->getIntegerForKey("TotalSpentGems", TotalSpentGems);
	MinedGems = d->getIntegerForKey("MinedGems", MinedGems);

    TotalSecondsPlayed = d->getIntegerForKey("TotalSecondsPlayed", TotalSecondsPlayed);
    FastRechargeBought = d->getBoolForKey("FastRechargeBought", FastRechargeBought);
    MeshUnlocked = d->getBoolForKey("MeshUnlocked", MeshUnlocked);
    NaturalsUnlocked = d->getBoolForKey("NaturalsUnlocked", NaturalsUnlocked);
    SnakeUnlocked = d->getBoolForKey("SnakeUnlocked", SnakeUnlocked);
    RaidUnlocked = d->getBoolForKey("RaidUnlocked", RaidUnlocked);
    TreeUnlocked = d->getBoolForKey("TreeUnlocked", TreeUnlocked);
    FallUnlocked = d->getBoolForKey("FallUnlocked", FallUnlocked);
	DronesArcadeUnlocked = d->getBoolForKey("DronesArcadeUnlocked", DronesArcadeUnlocked);
	DonkeyUnlocked = d->getBoolForKey("DonkeyUnlocked", DonkeyUnlocked);

    MeshPlayed = d->getIntegerForKey("MeshPlayed", MeshPlayed);
    NaturalsPlayed = d->getIntegerForKey("NaturalsPlayed", NaturalsPlayed);
    SnakePlayed = d->getIntegerForKey("SnakePlayed", SnakePlayed);
    RaidPlayed = d->getIntegerForKey("RaidPlayed", RaidPlayed);
    TreePlayed = d->getIntegerForKey("TreePlayed", TreePlayed);
    FallPlayed = d->getIntegerForKey("FallPlayed", FallPlayed);
    DronesArcadePlayed = d->getIntegerForKey("DronesArcadePlayed", DronesArcadePlayed);
	EggplantPlayed = d->getIntegerForKey("EggplantPlayed", EggplantPlayed);
	DonkeyPlayed = d->getIntegerForKey("DonkeyPlayed", DonkeyPlayed);

	PlayerR = d->getIntegerForKey("PlayerR", PlayerR);
	PlayerG = d->getIntegerForKey("PlayerG", PlayerG);
	PlayerB = d->getIntegerForKey("PlayerB", PlayerB);

	Melodie::MelodieScene::LoadSettings();
	Blob::BlobScene::LoadSettings();
	Matryoshka::MatryoshkaScene::LoadSettings();
	Contrast::ContrastScene::LoadSettings();

	SaveSettings();
}

void GameSettings::SaveSettings(void)
{
	Ooze::SaveSettings();
	auto d = UserDefault::getInstance();
	d->setIntegerForKey("checker", 667);

	d->setBoolForKey("MuteSounds", MuteSounds);
	d->setBoolForKey("IsMusicMuted", IsMusicMuted);
	d->setStringForKey("CurrentTrack", CurrentTrack);

	d->setBoolForKey("AutoController", AutoController);
	d->setIntegerForKey("CurrentEpisode", CurrentEpisode);
	d->setIntegerForKey("CurrentLevelId", CurrentLevelId);

	for (auto ep : UnlockedEpisodes)
		d->setBoolForKey(("UnlockedEpisode" + nToString(ep)).c_str(), true);
	for (auto ep : FinishedEpisodes)
		d->setBoolForKey(("FinishedEpisode" + nToString(ep)).c_str(), true);

	for (size_t i = 0; i < Episodes.size(); i++)
		d->setIntegerForKey(("UnlockedLevelId" + nToString((int) i)).c_str(), UnlockedLevelIds[i]);

	SAVE_SCORE(Drones);
	SAVE_SCORE(Mesh);
	SAVE_SCORE(Snake);
	SAVE_SCORE(Raid);
	SAVE_SCORE(Tree);
	SAVE_SCORE(Fall);
	SAVE_SCORE(Donkey);

	d->setIntegerForKey("TotalMoves", TotalMoves);
	d->setIntegerForKey("LastKills", LastKills);
    d->setIntegerForKey("TotalKills", TotalKills);
	d->setIntegerForKey("LastTime", LastTime);
	d->setIntegerForKey("LastMoves", LastMoves);
	d->setIntegerForKey("LastWins", LastWins);
    d->setIntegerForKey("TotalMenus", TotalMenus);
    
	d->setIntegerForKey("Gems", Gems);
    d->setIntegerForKey("RegeneratedGems", RegeneratedGems);
    d->setIntegerForKey("TotalEarnedGems", TotalEarnedGems);
    d->setIntegerForKey("TotalSpentGems", TotalSpentGems);
	d->setIntegerForKey("MinedGems", MinedGems);

    d->setIntegerForKey("TotalSecondsPlayed", TotalSecondsPlayed);
    d->setBoolForKey("FastRechargeBought", FastRechargeBought);
    d->setBoolForKey("MeshUnlocked", MeshUnlocked);
    d->setBoolForKey("NaturalsUnlocked", NaturalsUnlocked);
    d->setBoolForKey("SnakeUnlocked", SnakeUnlocked);
    d->setBoolForKey("RaidUnlocked", RaidUnlocked);
    d->setBoolForKey("TreeUnlocked", TreeUnlocked);
    d->setBoolForKey("FallUnlocked", FallUnlocked);
	d->setBoolForKey("DronesArcadeUnlocked", DronesArcadeUnlocked);
	d->setBoolForKey("DonkeyUnlocked", DonkeyUnlocked);

    d->setIntegerForKey("MeshPlayed", MeshPlayed);
    d->setIntegerForKey("NaturalsPlayed", NaturalsPlayed);
    d->setIntegerForKey("SnakePlayed", SnakePlayed);
    d->setIntegerForKey("RaidPlayed", RaidPlayed);
    d->setIntegerForKey("TreePlayed", TreePlayed);
    d->setIntegerForKey("FallPlayed", FallPlayed);
	d->setIntegerForKey("DronesArcadePlayed", DronesArcadePlayed);
	d->setIntegerForKey("EggplantPlayed", EggplantPlayed);
	d->setIntegerForKey("DonkeyPlayed", DonkeyPlayed);

	d->setIntegerForKey("PlayerR", PlayerR);
	d->setIntegerForKey("PlayerG", PlayerG);
	d->setIntegerForKey("PlayerB", PlayerB);
	d->flush();
    
    if (TotalSecondsPlayed > ADDICT_TIME_HIGH)
        ReportAchievement(ACHIEVEMENT_ADDICT_HIGH);
    else if (TotalSecondsPlayed > ADDICT_TIME_MEDIUM)
        ReportAchievement(ACHIEVEMENT_ADDICT_MEDIUM);
    else if (TotalSecondsPlayed > ADDICT_TIME_LOW)
        ReportAchievement(ACHIEVEMENT_ADDICT_LOW);
}

string GameSettings::GetLevelFilename()
{
	auto episode = Episodes[CurrentEpisode];
	return CCFileUtils::getInstance()->fullPathForFilename(episode[CurrentLevelId % episode.size()]);
}

void GameSettings::AdvanceLevel()
{
	auto episode = Episodes[CurrentEpisode];
	CurrentLevelId++;
	if (CurrentLevelId >= static_cast<int>(episode.size()))
	{
		if (CurrentEpisode == 0) ReportAchievement(ACHIEVEMENT_EPISODE_1);
		else if (CurrentEpisode == 1) ReportAchievement(ACHIEVEMENT_EPISODE_2);
		else if (CurrentEpisode == 2) ReportAchievement(ACHIEVEMENT_EPISODE_3);

		if (FinishedEpisodes.find(CurrentEpisode) == FinishedEpisodes.end())
		{
			GiveGems(10); //reward 10 gems for finishing a chapter
		}
		FinishedEpisodes.insert(CurrentEpisode);
		if (FinishedEpisodes.size() == Episodes.size())
		{
			ReportAchievement(ACHIEVEMENT_FINISH_GAME);
			DronesArcadeUnlocked = true;
		}
		CurrentLevelId = 0;
	}

	if (FinishedEpisodes.size() == Episodes.size())
	{
		int time = 0;
		int kills = 0;
		int moves = 0;
		bool error = false;
		auto data = UserDefault::getInstance();
		for (auto epId : FinishedEpisodes)
		{
			for (size_t i = 0; i < Episodes[epId].size(); i++)
			{
				int t = data->getIntegerForKey(("timee" + nToString((int) epId) + "l" + nToString((int) i)).c_str(), -1);
				int k = data->getIntegerForKey(("killse" + nToString((int) epId) + "l" + nToString((int) i)).c_str(), -1);
				int m = data->getIntegerForKey(("movese" + nToString((int) epId) + "l" + nToString((int) i)).c_str(), -1);
				if (t < 0 || k < 0 || m < 0)
				{
					log("Error summing stats at E" + nToString((int) epId) + "L" + nToString((int) i));
					error = true;
					break;
				}
				time += t;
				kills += k;
				moves += m;
			}
			if (error) break;
		}

		if (!error)
		{
			ReportScore(LEADERBOARD_MINMOVES, moves);
			ReportScore(LEADERBOARD_MINTIME, time);
			ReportScore(LEADERBOARD_MAXKILLS, kills);
		}
	}

	if (CurrentLevelId > UnlockedLevelIds[CurrentEpisode]) 
		UnlockedLevelIds[CurrentEpisode] = CurrentLevelId;
	SaveSettings();
}

int GameSettings::GetMaximumLevelId()
{
	return (int)Episodes[CurrentEpisode].size();
}

int GameSettings::GetUnlockedLevelId()
{
	return UnlockedLevelIds[CurrentEpisode];
}

void GameSettings::TryGiveUnlockAll()
{
    if (MeshPlayed > 0 &&
        NaturalsPlayed > 0 &&
        SnakePlayed > 0 &&
        RaidPlayed > 0 &&
        TreePlayed > 0 &&
        FallPlayed > 0)
        ReportAchievement(ACHIEVEMENT_UNLOCKED_ALL);
}

void GameSettings::GiveGems(int points)
{
    Gems += points;
    TotalEarnedGems += points;
    
    if (Gems >= 100) ReportAchievement(ACHIEVEMENT_GEM_RICH);
    if (TotalEarnedGems >= 100) ReportAchievement(ACHIEVEMENT_GEM_EARNER);
    
    ReportScore(LEADERBOARD_RICH, Gems);
}

bool GameSettings::SpendGems(int points)
{
    if (Gems < points) return false;
    Gems -= points;
    TotalSpentGems += points;
    if (TotalSpentGems >= 100) ReportAchievement(ACHIEVEMENT_GEM_SPENDER);
    return true;
}

void GameSettings::OozeGameStats()
{
    Ooze::InstantSend({
		{ "Event", "GameStats" },
        {"TotalSecondsPlayed", nToString(TotalSecondsPlayed)},
        /*{"Gems", nToString(Gems)},
        {"RegeneratedGems", nToString(RegeneratedGems)},
        {"TotalEarnedGems", nToString(TotalEarnedGems)},
        {"TotalSpentGems", nToString(TotalSpentGems)},
		{"MinedGems", nToString(MinedGems) },
        {"MeshPlayed", nToString(MeshPlayed)},
        {"NaturalsPlayed", nToString(NaturalsPlayed)},
        {"SnakePlayed", nToString(SnakePlayed)},
        {"RaidPlayed", nToString(RaidPlayed)},
        {"TreePlayed", nToString(TreePlayed)},
        {"FallPlayed", nToString(FallPlayed)},
        {"DronesArcadePlayed", nToString(DronesArcadePlayed)},
		{"EggplantPlayed", nToString(EggplantPlayed) },
		{ "DonkeyPlayed", nToString(DonkeyPlayed) }*/
    });
}

void GameSettings::Set(string name, int val)
{
	UserDefault::getInstance()->setStringForKey(name.c_str(), nToString(val));
}

void GameSettings::Set(string name, float val)
{
	UserDefault::getInstance()->setStringForKey(name.c_str(), nToString(val));
}

void GameSettings::Set(string name, double val)
{
	UserDefault::getInstance()->setStringForKey(name.c_str(), nToString(val));
}

void GameSettings::Set(string name, string val)
{
	UserDefault::getInstance()->setStringForKey(name.c_str(), val);
}

void GameSettings::Set(string name, bool val)
{
	UserDefault::getInstance()->setStringForKey(name.c_str(), nToString(val));
}

void GameSettings::Set(string name, FLOAT2 val)
{
	UserDefault::getInstance()->setStringForKey(name.c_str(), nToString(val));
}

void GameSettings::Set(string name, FLOAT3 val)
{
	UserDefault::getInstance()->setStringForKey(name.c_str(), nToString(val));
}

void GameSettings::Set(string name, FLOAT4 val)
{
	UserDefault::getInstance()->setStringForKey(name.c_str(), nToString(val));
}

void GameSettings::Set(string name, vector<string> val)
{
	auto d = UserDefault::getInstance();
	d->setIntegerForKey(("_vector__size_" + name).c_str(), static_cast<int>(val.size()));
	int i = -1;
	for (auto item : val)
	{
		i++;
		d->setStringForKey(("_vector__item" + nToString(i) + "_" + name).c_str(), item);
	}
}

int GameSettings::GetInt(string name, int def)
{
	return UserDefault::getInstance()->getIntegerForKey(name.c_str(), def);
}

float GameSettings::GetFloat(string name, float def)
{
	return UserDefault::getInstance()->getFloatForKey(name.c_str(), def);
}

double GameSettings::GetDouble(string name, double def)
{
	return UserDefault::getInstance()->getDoubleForKey(name.c_str(), def);
}

string GameSettings::GetStr(string name, string def)
{
	return UserDefault::getInstance()->getStringForKey(name.c_str(), def);
}

bool GameSettings::GetBool(string name, bool def)
{
	return UserDefault::getInstance()->getBoolForKey(name.c_str(), def);
}

FLOAT2 GameSettings::GetFLOAT2(string name, FLOAT2 def)
{
	return nToFloat2(UserDefault::getInstance()->getStringForKey(name.c_str(), nToString(def)));
}

FLOAT3 GameSettings::GetFLOAT3(string name, FLOAT3 def)
{
	return nToFloat3(UserDefault::getInstance()->getStringForKey(name.c_str(), nToString(def)));
}

FLOAT4 GameSettings::GetFLOAT4(string name, FLOAT4 def)
{
	return nToFloat4(UserDefault::getInstance()->getStringForKey(name.c_str(), nToString(def)));
}

vector<string> GameSettings::GetVector(string name, vector<string> def)
{
	auto d = UserDefault::getInstance();
	int c = d->getIntegerForKey(("_vector__size_" + name).c_str(), -1);
	if (c < 0) return def;
	
	def.clear();
	for (int i = 0; i < c; i++)
		def.push_back(d->getStringForKey(("_vector__item" + nToString(i) + "_" + name).c_str()));

	return def;
}

void GameSettings::Flush()
{
	UserDefault::getInstance()->flush();
}

void GameSettings::SetMuteMusic(bool s)
{
	IsMusicMuted = s;

	if (IsMusicMuted)
		SimpleAudioEngine::getInstance()->stopBackgroundMusic();
	else if (CurrentTrack != "")
		SimpleAudioEngine::getInstance()->playBackgroundMusic(CurrentTrack.c_str(), true);
}

void GameSettings::PlayMusic(string s)
{
	CurrentTrack = s;
	if (!IsMusicMuted)
		SimpleAudioEngine::getInstance()->playBackgroundMusic(CurrentTrack.c_str(), true);
}

void GameSettings::StopMusic()
{
	CurrentTrack = "";
	SimpleAudioEngine::getInstance()->stopBackgroundMusic();
}