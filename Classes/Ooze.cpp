#include "Ooze.h"

vector<string> Ooze::sendQueue;
vector<map<string, string>> Ooze::reliableSendQueue;
set<string> Ooze::acks;

int Ooze::msgId = 0;
string Ooze::Alias = OOZE_DEFAULTALIAS;
int Ooze::GameId = -1;
int Ooze::InstanceId = rand();
string Ooze::InstallId = OOZE_DEFAULTINSTALLID;
string Ooze::Url = "http://localhost:700";

//read the contents of this file and set Ooze::Url to that.
static const string OOZE_URL_FILE = "ooze.txt";

const ccHttpRequestCallback Ooze::ResponseCame = 
[](HttpClient* client, HttpResponse* response)
{
	std::vector<char> *buffer = response->getResponseData();
	std::vector<char> *headerBuffer = response->getResponseHeader();

	string result = "";
	string header = "";

	for (auto c : *headerBuffer) header += nToString(c);
	for (auto c : *buffer) result += nToString(c);

#if defined(WIN32)
	/*log("HEADER:" + nToString((int)header.size()) + "---");
	log(header);
	log("---ENDHEADER");*/

	log("Data Received:" + nToString((int)result.size()) + "----");
	log(result);
	log("---:EndData");
#endif

	auto messages = Ooze::StrToMapList(result);
	size_t oldCount = Ooze::reliableSendQueue.size();

	for (auto message : messages)
	{
		if (CONTAINS(message, "Event"))
		{
			if (message["Event"] == "respondAcks")
			{
				int acklen = nToInt(message["RESPONDACKSLEN"]);
				for (int i = 0; i < acklen; i++)
					Ooze::acks.insert(message["RESPONDACK" + nToString(i)]);
			
				for (auto ack : Ooze::acks)
				{
					for (auto itr = Ooze::reliableSendQueue.begin(); itr != Ooze::reliableSendQueue.end(); itr++)
						if ((*itr)["ACK_CODE"] == ack)
						{
							Ooze::reliableSendQueue.erase(itr);
							break;
						}
				}
			}
			else if (message["Event"] == "getHighScores")
			{
				int scorelen = nToInt(message["HIGHSCORELEN"]);
				string category = message["Game"];

				//GameSettings::LeaderBoard.clear();
				vector<ScorePair> lb;
				GameSettings::GlobalRanking = -1;
				GameSettings::GlobalTotalScore = -1;
				for (int i = 0; i < scorelen; i++)
				{
					ScorePair sp;
					sp.Name = message[string_format("HIGHSCORE%dalias", i)];
					sp.Score = nToInt(message[string_format("HIGHSCORE%dhighscore", i)]);
					sp.TotalScore = nToInt(message[string_format("HIGHSCORE%dtotalscore", i)]);
					sp.InstallId = message[string_format("HIGHSCORE%dinstallid", i)];
					lb.push_back(sp);

					log(string_format("[%s] - [%s] (%s, %d)", 
						sp.InstallId.c_str(), Ooze::InstallId.c_str(),
						sp.Name.c_str(), sp.TotalScore));
					if (sp.InstallId == Ooze::InstallId)
					{
						log("OK");
						GameSettings::GlobalTotalScore = sp.TotalScore;
						GameSettings::GlobalRanking = i + 1;
						if (i > 0)
							GameSettings::GlobalLevelupDelta =
							(lb.rbegin() + 1)->TotalScore - sp.TotalScore;
						else
							GameSettings::GlobalLevelupDelta = -1;
					}
				}

				GameSettings::LeaderBoards[category] = lb;
			}
		}

		if (oldCount != Ooze::reliableSendQueue.size())
		{
			Ooze::SerializeReliableQueue();
			Ooze::SaveSettings();
		}
	}
};

void Ooze::SaveSettings()
{
	auto d = UserDefault::getInstance();
	d->setStringForKey("OozeAlias", Alias);
	d->setStringForKey("OozeInstallId", InstallId);
	d->flush();
}

void Ooze::LoadSettings()
{
	if (FileUtils::getInstance()->isFileExist(OOZE_URL_FILE))
		Ooze::Url = FileUtils::getInstance()->getStringFromFile(OOZE_URL_FILE);
	else log("Ooze URL file not found.");

	auto d = UserDefault::getInstance();
	Alias = d->getStringForKey("OozeAlias", Alias);
#ifdef WIN32
	InstallId = "0";
#else
	InstallId = d->getStringForKey("OozeInstallId", nToString(rand()));
#endif
	
	SaveSettings();
	//DeserializeReliableQueue();
}

void Ooze::SerializeReliableQueue()
{
	auto d = UserDefault::getInstance();
	d->setIntegerForKey("OozeReliableQueueLen", (int) reliableSendQueue.size());
	int i = 0;
	for (auto message : reliableSendQueue)
	{
		string s = MapToStr(message);
		d->setStringForKey(string_format("OozeReliableQueueItem%d", i).c_str(), s);
		i++;
	}
}

void Ooze::DeserializeReliableQueue()
{
	auto d = UserDefault::getInstance();
	int count = d->getIntegerForKey("OozeReliableQueueLen", 0);
	for (int i = 0; i < count; i++)
	{
		auto m = StrToMap(d->getStringForKey(string_format("OozeReliableQueueItem%d", i).c_str()));
		reliableSendQueue.push_back(m);
	}
}

void Ooze::AddMetadata(map<string, string>& kp)
{
	kp["GAME_NAME"] = GameName;
	kp["ALIAS"] = Alias;
	kp["INSTANCE_ID"] = nToString(InstanceId);
	kp["GAME_ID"] = nToString(GameId);
	kp["INSTALL_ID"] = (InstallId);
	kp["TIME"] = nToString((int) time(nullptr));
	kp["MSG_ID"] = nToString(msgId++);
}

#if !defined(DISABLE_OOZE)

void Ooze::Queue(map<string, string> kp)
{
	AddMetadata(kp);
	sendQueue.push_back(MapToStr(kp));
}

void Ooze::Flush()
{
	for (auto msg : reliableSendQueue) sendQueue.push_back(MapToStr(msg));
	if (sendQueue.empty()) return;
/*
	for (auto ack : acks)
		sendQueue.push_back(MapToStr({
			{ "Event", "AckBack" },
			{ "INSTALL_ID", (InstallId) },
			{ "ACK_CODE", ack },
			{ "ACK_REQ", "2" } }));
*/
	string data = sendQueue[0];
	for (auto itr = sendQueue.begin() + 1; itr != sendQueue.end(); itr++)
		data += "\r\n" + *itr;

	HttpRequest* request = new HttpRequest();
	request->setUrl(OOZE_URL.c_str());
	request->setRequestType(HttpRequest::Type::POST);
	request->setResponseCallback(Ooze::ResponseCame);
	//log("Sending " + data);
	request->setRequestData(data.c_str(), data.size());
	HttpClient::getInstance()->send(request);
	request->release();

	sendQueue.clear();

	msgId++;
	log("Data Sent.");
}

#else

void Ooze::Queue(map<string, string> kp)
{
	sendQueue.push_back(MapToStr(kp));
}

void Ooze::Flush()
{
	for (auto sq : sendQueue)
		log(sq);
	sendQueue.clear();
}
#endif

string Ooze::MapToStr(map<string, string> data)
{
	string result = "";
	vector<string> lines;
	for (auto kv : data)
		lines.push_back(kv.first + "=" + kv.second);

	if (lines.size() == 1) result = lines[0];
	else if (lines.size() > 1)
	{
		result = lines[0];
		for (auto itr = lines.begin() + 1; itr != lines.end(); itr++)
			result += "&" + *itr;
	}

	return result;
}

map<string, string> Ooze::StrToMap(string s)
{
	map<string, string> o;
	if (nTrim(s).size() == 0) return o;
	if (s.find("=") == s.npos) return o;

	auto keyvals = nSplit(s, '&');
	
	for (auto keyval : keyvals)
	{
		auto pair = nSplit(keyval, '=');
		if (pair.size() != 2) continue;
		o[pair[0]] = pair[1];
	}

	return o;
}

vector<map<string, string>> Ooze::StrToMapList(string s)
{
	vector<map<string, string>> o;

	if (nTrim(s).size() == 0) return o;
	if (s.find("=") == s.npos) return o;

	auto reqs = nSplit(nReplaceAll(s, "\r\n", "\r"), "\r");
	
	for (auto req : reqs)
	{
		auto m = StrToMap(req);
		if (m.size() > 0) o.push_back(m);
	}

	return o;
}

void Ooze::ReliableSend(map<string, string> data)
{
	InstantSend(data);
	return;
/*
	data["ACK_CODE"] = 
		nToString((int) time(nullptr)) + 
		nToString(rand()) + 
		nToString(msgId);
	data["ACK_REQ"] = "1";
	AddMetadata(data);
	reliableSendQueue.push_back(data);

	SerializeReliableQueue();

	Flush();*/
}

void Ooze::InstantSend(map<string, string> data)
{
	data["ACK_CODE"] =
		nToString((int) time(nullptr)) +
		nToString(rand()) +
		nToString(msgId);
	data["ACK_REQ"] = "0";
	Queue(data);
	Flush();
}

void Ooze::SendString(string data)
{
	sendQueue.push_back(data);
	Flush();
}