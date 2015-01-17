#ifndef __OOZE_H__
#define __OOZE_H__

#include "helpers.h"

#include "network/HttpClient.h"
#include "network/HttpRequest.h"
using namespace cocos2d::network;

#define OOZE_URL		Ooze::Url 

#define OOZE_DEFAULTALIAS "Anonymous"
#define OOZE_DEFAULTINSTALLID "ERR"

class Ooze
{
public:
	static vector<string> sendQueue;
	static vector<map<string, string>> reliableSendQueue;
	static set<string> acks;

	static int msgId;

	static void SaveSettings();
	static void LoadSettings();

	static void SerializeReliableQueue();
	static void DeserializeReliableQueue();
	static string GameName;
    static int GameId, InstanceId;
    static string InstallId;
	static string Alias;
	static string Url;
	static void ReliableSend(map<string, string> data);
	static void InstantSend(map<string, string> data);
	static void SendString(string data);
	static void Queue(map<string, string> data);
	static void AddMetadata(map<string, string>& data);
	static void Flush();
	static string MapToStr(map<string, string>);
	static map<string, string> StrToMap(string);
	static vector<map<string, string>> StrToMapList(string);
	static const ccHttpRequestCallback ResponseCame;
};

#endif