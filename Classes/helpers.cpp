#include "helpers.h"
#include "SimpleAudioEngine.h"
#include "tinyxml2/tinyxml2.h"

bool AdsEnabled = true;

MenuItemSprite* MakeButton(string spriteUp, string spriteDn, string text, float fontSize, function<void(Ref*)> f, string fontface)
{
	auto up = MAKESPRITE(spriteUp);
	auto lblUp = Label::createWithTTF(text, fontface, fontSize);
	auto upRect = up->getContentSize();
	lblUp->setPosition(Point(upRect.width * 0.5f, upRect.height * 0.5f));
	up->addChild(lblUp);

	auto dn = MAKESPRITE(spriteDn);
	auto lblDn = Label::createWithTTF(text, fontface, fontSize);
	auto dnRect = dn->getContentSize();
	lblDn->setPosition(Point(dnRect.width * 0.5f, dnRect.height * 0.5f));
	dn->addChild(lblDn);

	return MenuItemSprite::create(up, dn, f);
}

MenuItemSprite* MakeButtonTextBelow(string spriteUp, string spriteDn, string text,
    float fontSize, function<void(Ref*)> f, string fontface, COLOR3B textColor)
{
    auto up = MAKESPRITE(spriteUp);
    auto lblUp = Label::createWithTTF(text, fontface, fontSize);
    auto upRect = up->getContentSize();
    lblUp->setPosition(Point(upRect.width * 0.5f, -upRect.height * 0.25f));
    up->addChild(lblUp);
    
    auto dn = MAKESPRITE(spriteDn);
    auto lblDn = Label::createWithTTF(text, fontface, fontSize);
    auto dnRect = dn->getContentSize();
    lblDn->setPosition(Point(dnRect.width * 0.5f, -dnRect.height * 0.25f));
    dn->addChild(lblDn);
    
    lblUp->setColor(textColor);
    lblDn->setColor(textColor);
    
    return MenuItemSprite::create(up, dn, f);
}

MenuItemSprite* MakeButton(string sprite, string text, float fontSize, function<void(Ref*)> f)
{
	auto up = MAKESPRITE(sprite);
	auto lblUp = Label::createWithTTF(text, FANCY_TTF, fontSize);
	auto upRect = up->getContentSize();
	lblUp->setPosition(Point(upRect.width * 0.5f, upRect.height * 0.5f));
	up->addChild(lblUp);

	auto dn = MAKESPRITE(sprite);
	dn->setColor(Color3B(170, 170, 170));
	auto lblDn = Label::createWithTTF(text, FANCY_TTF, fontSize);
	auto dnRect = dn->getContentSize();
	lblDn->setPosition(Point(dnRect.width * 0.5f, dnRect.height * 0.5f));
	dn->addChild(lblDn);

	return MenuItemSprite::create(up, dn, f);
}

MenuItemSprite* MakeButton(string sprite, float fontSize, function<void(Ref*)> f)
{
	auto up = MAKESPRITE(sprite);
	auto dn = MAKESPRITE(sprite);
	dn->setColor(Color3B(170, 170, 170));
	return MenuItemSprite::create(up, dn, f);
}

void log(const string& s)
{
    log(s.c_str());
}

int Position::width = 0;
int Position::height = 0;
bool Position::operator < (const Position b_) const
{
	//return x + width*y < b_.x + width*b_.y;
	if (y < b_.y) return true;
	if (y == b_.y) return x < b_.x;
	return false;
}

bool Position::operator == (const Position b_) const
{
	return x == b_.x && y == b_.y;
}

void PlayEffect(const string sound, float pitch, float gain)
{
	if (GameSettings::MuteSounds) return;
#ifdef WINRT
	const char * path = CCFileUtils::sharedFileUtils()->fullPathForFilename("sound.wav").c_str();
#else
	const char * path = sound.c_str();
#endif
	if (strlen(path))
		CocosDenshion::SimpleAudioEngine::getInstance()->playEffect(path, false, pitch, 0, gain);
}

void PlayEffect(const vector<string> sounds, float pitch, float gain)
{
	PlayEffect(sounds[rand() % sounds.size()], pitch, gain);
}

static tinyxml2::XMLElement* getXMLNodeForKey(const char* pKey, tinyxml2::XMLElement** rootNode, tinyxml2::XMLDocument **doc, string filename)
{
	tinyxml2::XMLElement* curNode = nullptr;

	// check the key value
	if (!pKey)
	{
		return nullptr;
	}

	do
	{
		tinyxml2::XMLDocument* xmlDoc = new tinyxml2::XMLDocument();
		*doc = xmlDoc;

		std::string xmlBuffer = FileUtils::getInstance()->getStringFromFile(filename);

		if (xmlBuffer.empty())
		{
			CCLOG("can not read xml file");
			break;
		}
		xmlDoc->Parse(xmlBuffer.c_str(), xmlBuffer.size());

		// get root node
		*rootNode = xmlDoc->RootElement();
		if (nullptr == *rootNode)
		{
			CCLOG("read root node error");
			break;
		}
		// find the node
		curNode = (*rootNode)->FirstChildElement();
		while (nullptr != curNode)
		{
			const char* nodeName = curNode->Value();
			if (!strcmp(nodeName, pKey))
			{
				break;
			}

			curNode = curNode->NextSiblingElement();
		}
	} while (0);

	return curNode;
}

string getStringForKey(const char* pKey, const string& defaultValue)
{
#ifdef FA
	return getStringForKey(pKey, defaultValue, "fa.xml");
#else
	return getStringForKey(pKey, defaultValue, "text.xml");
#endif
}
string getStringForKey(const char* pKey, const string& defaultValue, const string& filename)
{
	const char* value = nullptr;
	tinyxml2::XMLElement* rootNode;
	tinyxml2::XMLDocument* doc;
	tinyxml2::XMLElement* node;
	node = getXMLNodeForKey(pKey, &rootNode, &doc, filename);
	// find the node
	if (node && node->FirstChild())
	{
		value = (const char*) (node->FirstChild()->Value());
	}

	string ret = defaultValue;

	if (value)
	{
		ret = string(value);
	}

	if (doc) delete doc;

	return ret;
}

char getCharForKeyCode(Key_t k, bool shift)
{
	if (k >= Key_t::KEY_A && k <= Key_t::KEY_Z) 
		return (shift ? 'A' : 'a') + (char) (k) - (char)(Key_t::KEY_A);
	if (k >= Key_t::KEY_CAPITAL_A && k <= Key_t::KEY_CAPITAL_Z) 
		return (shift ? 'A' : 'a') + (char) (k) - (char) (Key_t::KEY_CAPITAL_A);
	if (!shift)
	{
		if (k >= Key_t::KEY_0 && k <= Key_t::KEY_9)
			return '0' + (char) (k) -(char) (Key_t::KEY_0);
	}
	if (k == Key_t::KEY_KP_PLUS || k == Key_t::KEY_PLUS || k == Key_t::KEY_EQUAL)
		return shift ? '+' : '=';
	if (k == Key_t::KEY_KP_DIVIDE || k == Key_t::KEY_SLASH)
		return shift ? '?' : '/';
	if (k == Key_t::KEY_KP_MULTIPLY || k == Key_t::KEY_ASTERISK || k == Key_t::KEY_8)
		return '*';
	if (k == Key_t::KEY_KP_MINUS || k == Key_t::KEY_MINUS)
		return shift ? '_' : '-';
	if (k == Key_t::KEY_SPACE)
		return ' ';
	if (k == Key_t::KEY_QUOTE || k == Key_t::KEY_APOSTROPHE)
		return shift ? '\"' : '\'';
	if (k == Key_t::KEY_EXCLAM || k == Key_t::KEY_1)
		return '!';
	if (k == Key_t::KEY_AT || k == Key_t::KEY_2)
		return '@';
	if (k == Key_t::KEY_NUMBER || k == Key_t::KEY_3)
		return '#';
	if (k == Key_t::KEY_DOLLAR || k == Key_t::KEY_4)
		return '$';
	if (k == Key_t::KEY_PERCENT || k == Key_t::KEY_5)
		return '%';
	if (k == Key_t::KEY_CIRCUMFLEX || k == Key_t::KEY_6)
		return '^';
	if (k == Key_t::KEY_AMPERSAND || k == Key_t::KEY_7)
		return '&';
	if (k == Key_t::KEY_LEFT_PARENTHESIS || k == Key_t::KEY_9)
		return '(';
	if (k == Key_t::KEY_RIGHT_PARENTHESIS || k == Key_t::KEY_0)
		return ')';
	if (k == Key_t::KEY_COMMA)
		return shift ? '<' : ',';
	if (k == Key_t::KEY_PERIOD)
		return shift ? '>' : '.';
	if (k == Key_t::KEY_COLON || k == Key_t::KEY_SEMICOLON)
		return shift ? ':' : ';';
	if (k == Key_t::KEY_LESS_THAN)
		return '<';
	if (k == Key_t::KEY_GREATER_THAN)
		return '>';
	if (k == Key_t::KEY_QUESTION)
		return '?';
	if (k == Key_t::KEY_BACK_SLASH)
		return '\\';
	if (k == Key_t::KEY_LEFT_BRACKET)
		return shift ? '{' : '[';
	if (k == Key_t::KEY_RIGHT_BRACKET)
		return shift ? '}' : ']';
	if (k == Key_t::KEY_LEFT_BRACE)
		return '{';
	if (k == Key_t::KEY_RIGHT_BRACE)
		return '}';
	if (k == Key_t::KEY_TILDE)
		return shift ? '~' : '`';

	return (char)0;
}

string LS(string string_code)
{
	string result = string_code;
	for (auto kp : GameSettings::LocalizedStrings)
	{
		if (kp.first == string_code) return kp.second;
	}
	result = getStringForKey(string_code.c_str(), result);
	GameSettings::LocalizedStrings[string_code] = result;
	return result;
}

void recursivePause(Node* base)
{
	recursiveAction(base, [](Node* node){ node->pause(); });
}

void recursiveResume(Node* base)
{
	recursiveAction(base, [](Node* node){ node->resume(); });
}

void recursiveAction(Node* base, function<void(Node*)> f, bool runonbase)
{
	if (!base) return;
	stack<Node*> nodes;
	nodes.push(base);
	while (!nodes.empty())
	{
		auto node = nodes.top();
		nodes.pop();

		for (auto child : node->getChildren()) nodes.push(child);
		if (node == base)
		{
			if (runonbase) f(node);
		}
		else f(node);
	}
}

#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)
void ShowFullscreenAd(){
	log("Showing interstitial add in android");
	SendMessageWithParams(string("ShowAdmobInter"), NULL);
}
void OpenFacebookPage(){
	log("Opening facebook page in android");
	SendMessageWithParams(string("OpenFacebookPage"), NULL);
}
void OpenNaturalsPage(){
	log("Opening naturals page in android");
	SendMessageWithParams(string("OpenNaturalsPage"), NULL);
}
void OpenStorePage(){
	log("Opening store page in android");
	SendMessageWithParams(string("OpenStorePage"), NULL);
}
void BuyPro(){
	log("Opening buy page in android");
	SendMessageWithParams(string("BuyPro"), NULL);
}
void ShareScore(int i, Games game)
{
	log("Sharing score in android");
	CCDictionary* prms = CCDictionary::create();
	prms->setObject(CCString::create(nToString(i).c_str()), "score");
	prms->setObject(CCString::create(GameInfos[game].title.c_str()), "game");
	prms->setObject(CCString::create(GameInfos[game].pictureUrl.c_str()), "picture");
	SendMessageWithParams(string("ShareScore"), prms);
}
void CreateBannerAd()
{
	CCDictionary* prms = CCDictionary::create();

#if defined(BANNER_ON_BOTTOM)
	prms->setObject(CCString::create("true"), "BannerOnBottom");
#endif

	SendMessageWithParams(string("CreateBannerAd"), prms);
}
void OpenUrl(string url)
{
	log("Opening url in android: " + url);
	CCDictionary* prms = CCDictionary::create();
	prms->setObject(CCString::create(url.c_str()), "url");
	SendMessageWithParams(string("OpenUrl"), prms);
}
#endif
