#include "helpers.h"
#include "GameCenter.h"
#include "GameSettings.h"
#include "Ooze.h"

#if IS_IOS
GameKitDelegate GameSettings::GameKit;
#endif
function<void(string)> GameSettings::GameCenterDataReceived = [](string category)
{
	log("Scores Recieved");
	for (auto sp : GameSettings::GetLeaderBoard(category))
		log(string_format("%s: %d", sp.Name.c_str(), sp.Score));
};

const bool ScorePair::operator==(const ScorePair b) const
{
	return 
		//Name == b.Name &&
		Score == b.Score && 
		TotalScore == b.TotalScore &&
		InstallId == b.InstallId;
}

string GameSettings::DefaultLeaderBoard;
map<string, vector<ScorePair>> GameSettings::LeaderBoards;

int GameSettings::GlobalRanking = -1;
int GameSettings::GlobalTotalScore = -1;
int GameSettings::GlobalLevelupDelta = -1;

map<string, string> GameSettings::PlayerNames;

vector<ScorePair> GameSettings::GetLeaderBoard(string category)
{
	if (LeaderBoards.find(category) != LeaderBoards.end())
		return LeaderBoards[category];

	return vector < ScorePair > {};
}

vector<ScorePair> GameSettings::GetLeaderBoard()
{
	return GetLeaderBoard(DefaultLeaderBoard);
}

void GameSettings::InitGameCenter(Games game)
{
#if IS_IOS
    GKHWrapperCpp gkh;
    gkh.setDelegate(&GameKit);
    gkh.authenticateLocalPlayer();
    string currentAlias = gkh.getLocalPlayerName();
	if (currentAlias != OOZE_DEFAULTALIAS) Ooze::Alias = currentAlias;
    string playerId = gkh.getLocalPlayerId();
	if (playerId != "" && playerId != OOZE_DEFAULTINSTALLID) Ooze::InstallId = playerId;
    FetchGameCenterData(game);
#endif
}

void GameSettings::FetchGameCenterData(Games game)
{/*
#if IS_IOS && defined(GAME_CENTER_LEADERBOARDS)
    GKHWrapperCpp gkh;
    gkh.getLocalPlayerFriends();
#if defined(S_GAME)
	gkh.retrieveTopTenAllTimeGlobalScores(GameInfos[game].leaderboardId);
#else
    gkh.retrieveTopTenAllTimeGlobalScores("grp.DronesArcadeScoreLeaderboard");
#endif
#else
    */
	Ooze::InstantSend({
			{ "Event", "GetScores" },
			{ "Game", GameInfos[game].leaderboardId 
#if defined(FA)
			+ "FA"
#endif
			}
	});
//#endif
}

void GameSettings::ReportScore(string category, int score)
{
#if IS_IOS
	GKHWrapperCpp gkh;
	gkh.submitScoreToCatagory(score, category);
    string currentAlias = gkh.getLocalPlayerName();
    if (currentAlias != "Anonymous") Ooze::Alias = currentAlias;
    string playerId = gkh.getLocalPlayerId();
    if (playerId != "" && playerId != "ERR") Ooze::InstallId = playerId;
#endif
	Ooze::ReliableSend({
			{ "Event", "ReportScore" },
			{ "Alias", Ooze::Alias },
			{ "Game", category
			},
			{ "Score", nToString(score) }
	});
}

void GameSettings::ReportScore(Games game, int score)
{
	log(string_format("Reporting score %s: %d", GameInfos[game].title.c_str(), score));
#if IS_IOS
	GKHWrapperCpp gkh;
	gkh.submitScoreToCatagory(score, GameInfos[game].leaderboardId);
    string currentAlias = gkh.getLocalPlayerName();
    if (currentAlias != "Anonymous") Ooze::Alias = currentAlias;
#endif
	Ooze::ReliableSend({
			{ "Event", "ReportScore" },
			{ "Alias", Ooze::Alias },
			{ "Game", GameInfos[game].leaderboardId
#if defined(FA)
			+"FA"
#endif 
			},
			{ "Score", nToString(score) }
	});
}

void GameSettings::ReportAchievement(string name, float percentComplete, bool showCompletion)
{
#if IS_IOS
    GKHWrapperCpp gkh;
    gkh.reportAchievement(name, percentComplete, showCompletion);
#endif
	Ooze::ReliableSend({
			{ "Event", "ReportAchievement" },
			{ "Name", name },
			{ "PercentComplete", nToString(percentComplete) },
			{ "ShowCompletion", nToString(showCompletion) },
			{ "Application", GameInfos[APPLICATION].title }
	});
}

void GameSettings::GetPlayersInfo(std::vector<std::string> friends)
{
#if IS_IOS
    vector<string> idsToQuery;
    for (auto f : friends)
    {
        if (GameSettings::PlayerNames.find(f) ==
            GameSettings::PlayerNames.end())
        {
            idsToQuery.push_back(f);
        }
    }
    if (!idsToQuery.empty())
    {
        GKHWrapperCpp gkh;
        gkh.getPlayerInfo(idsToQuery);
    }
#endif
}

string GameSettings::GetPlayerName(std::string p)
{
    auto itr = PlayerNames.find(p);
    if (itr != PlayerNames.end())
    {
        return (*itr).second;
    }
//#if IS_IOS
//    log (p + " not found.");
//#endif
    return p;
}

int GameSettings::GetHighScore()
{
#if !IS_IOS
	return DronesHighScore;
#else
	if (GetLeaderBoard().empty()) return DronesHighScore;
	return GetLeaderBoard()[0].Score;
#endif
}


#if IS_IOS
void GameKitDelegate::onScoresReceived(vector<GKScoreCpp> scores)
{
    log("Scores Received from GameKit.");
    return; //DISABLE
    /*GameSettings::LeaderBoard.clear();
    vector<string> idsToQuery;
    for (auto item : scores)
    {
        log("Score received for player " + item.playerID);
        //playerID is some weird code
        GameSettings::LeaderBoard.push_back({item.playerID, (int)(item.value)});
        if (GameSettings::PlayerNames.find(item.playerID) ==
            GameSettings::PlayerNames.end())
        {
            idsToQuery.push_back(item.playerID);
        }
    }
    if (!idsToQuery.empty())
    {
        GKHWrapperCpp gkh;
        gkh.getPlayerInfo(idsToQuery);
    }
    if (GameSettings::GameCenterDataReceived)
    {
        log("Calling GameCenterDataReceived.");
        GameSettings::GameCenterDataReceived();
    }*/
}

void GameKitDelegate::onLocalPlayerAuthenticationChanged()
{
    GKHDelegate::onLocalPlayerAuthenticationChanged();
    GameSettings::FetchGameCenterData(APPLICATION);
}

void GameKitDelegate::onFriendsListRecieved(std::vector<std::string> friends)
{
    GKHDelegate::onFriendsListRecieved(friends);
    GameSettings::GetPlayersInfo(friends);
}

void GameKitDelegate::onPlayerInfoReceived(std::vector<GKPlayerCpp> playerInfo)
{
    /*for (auto player : playerInfo)
    {
        GameSettings::PlayerNames[player.playerID] = player.displayName;
    }
    if (GameSettings::GameCenterDataReceived)
    {
        log("Calling GameCenterDataReceived.");
        GameSettings::GameCenterDataReceived();
    }*/
}
#endif