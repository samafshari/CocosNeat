#ifndef __HELPERS_H
#define __HELPERS_H

#include <cmath>
#include <string>
#include <vector>
#include <array>
#include <map>
#include <iostream>
#include <sstream>
#include <fstream>
#include <functional>
#include "base/CCEventKeyboard.h"

#include "cocos2d.h"
#include "TopAnchor.h"
#include "Neat.h"
#include "SimpleAudioEngine.h"
#include "GameInfo.h"

using namespace std;
USING_NS_CC;
using namespace Neat;
using namespace CocosDenshion;

#define IS_IOS (CC_TARGET_PLATFORM == CC_PLATFORM_IOS)
#define PTM_RATIO (32.0f)

#ifdef FA
#define NORMAL_FNT	"naskh.fnt"
#define NORMAL_TTF	"droidnaskh_regular.ttf"
#define BOLD_TTF NORMAL_TTF
#define FANCY_TTF	"droidnaskh_regular.ttf"
#define FIXED_FNT	"naskh.fnt"
#define MATH_FNT	"farsinum.fnt"

//hack for farsi, ttf doesn't show correctly
#define createWithTTF createWithSystemFont
#else
#define NORMAL_FNT	"gothic.fnt"
#define NORMAL_TTF	"gothic.ttf"
#define BOLD_TTF	"gothicb.ttf"
#define FANCY_TTF	"font.ttf"
#define FIXED_FNT	"fixed.fnt"
#define MATH_FNT FIXED_FNT
#endif

#define FIXED_FNT_ENGLISH "fixed.fnt"


#if IS_IOS
#define SS @
#define SS_t NSString*
#else
#define SS
#define SS_t std::string
#endif

MenuItemSprite* MakeButton(string spriteUp, string spriteDn, string text, float fontSize, function<void(Ref*)> f, string fontface = NORMAL_TTF);
MenuItemSprite* MakeButtonTextBelow(string spriteUp, string spriteDn, string text, float fontSize, function<void(Ref*)> f, string fontface = NORMAL_TTF, COLOR3B textColor=
#if defined(LIGHT_THEME)
     COLOR3B::BLACK
#else
     COLOR3B::WHITE
#endif
     );
MenuItemSprite* MakeButton(string sprite, string text, float fontSize, function<void(Ref*)> f);
MenuItemSprite* MakeButton(string sprite, float fontSize, function<void(Ref*)> f);

extern float ScaleFactor;
extern float DESIGN_X;
extern float DESIGN_Y;
extern float ASPECT_RATIO;
extern cocos2d::Size FRAME_SIZE;
extern cocos2d::Size VIEW_SIZE;
extern float VIEW_WIDTH, VIEW_HEIGHT;
extern float WINDOW_WIDTH, WINDOW_HEIGHT;
extern float VIEW_OFFSET_Y;
extern int DPI;

#define PX(x) (static_cast<float>(x) * DESIGN_X)
#define PY(y) (VIEW_OFFSET_Y + static_cast<float>(y) * DESIGN_Y)
#define CP(x,y) (Point(PX((x)), PY((y))))

#define VPX(x) (static_cast<float>(x) * (VIEW_WIDTH/WINDOW_WIDTH) * DESIGN_X)
#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID) && defined(ADMOB) && !defined(NOBANNER)
#define VPY(y) (VIEW_OFFSET_Y + static_cast<float>(y) * (VIEW_HEIGHT/WINDOW_HEIGHT) * DESIGN_Y * 0.92f)
#else
#define VPY(y) (VIEW_OFFSET_Y + static_cast<float>(y) * (VIEW_HEIGHT/WINDOW_HEIGHT) * DESIGN_Y)
#endif
#define VCP(x,y) (Point(VPX((x)), VPY((y))))

extern bool AdsEnabled;
extern void ShowFullscreenAd();
extern void OpenFacebookPage();
extern void OpenNaturalsPage();
extern void OpenStorePage();
extern void OpenRatePopup();
extern void ShareScore(int=0, Games=Games::Drones);
extern void BuyPro();
extern void OpenUrl(string);
#ifdef WIN32
inline void ShowFullscreenAd(){ log("FullscreenAd not impld."); }
inline void OpenFacebookPage(){ log("OpenFacebookPage not impld."); }
inline void OpenNaturalsPage(){ log("OpenNaturalsPage not impld."); }
inline void OpenStorePage(){ log("OpenStorePage not impld."); }
inline void OpenRatePopup(){ log("OpenRatePopup not impld.");}
inline void ShareScore(int, Games) { log("ShareScore not impld."); }
inline void BuyPro() { log("BuyPro not impld."); }
inline void OpenUrl(string) { log("OpenUrl not impld."); }
#endif

#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)
extern void CreateBannerAd();
inline void OpenRatePopup() { OpenStorePage(); }
#endif


#define LOADATLAS(batch, sheet) \
	{ \
		string sheetName = cocos2d::FileUtils::getInstance()->fullPathForFilename(sheet); \
		assert(cocos2d::FileUtils::getInstance()->isFileExist(sheetName)); \
	}\
	SpriteBatchNode* (batch) = SpriteBatchNode::create(sheet);

#define FRAME(batch, name, x, y, w, h) \
	SpriteFrameCache::getInstance()->addSpriteFrame( \
	SpriteFrame::createWithTexture( \
	batch->getTexture(), \
	Rect( \
	(int) ((x)*ScaleFactor), \
	(int) ((y)*ScaleFactor), \
	(int) ((w)*ScaleFactor), \
	(int) ((h)*ScaleFactor) \
	), false, Point::ZERO, \
	Size((w)*ScaleFactor, (h)*ScaleFactor)), (name));

#define GETFRAME(name) SpriteFrameCache::getInstance()->getSpriteFrameByName(name)
#define MAKESPRITE(name) Sprite::createWithSpriteFrameName((name))
#define MAKEBUTTON(btnname, pbtnname, action) \
	MenuItemSprite::create(MAKESPRITE((btnname)), MAKESPRITE((pbtnname)), [&](Ref *sender){action(sender);})

#include "GameSettings.h"

void log(const string& s);

void PlayEffect(const std::string sound, float pitch = 1.0f, float gain = 1.0f);
void PlayEffect(const std::vector<std::string> sounds, float pitch = 1.0f, float gain = 1.0f);

enum class Directions
{
	Stop = 0,
	Left = 1,
	Right = 2,
	Up = 3,
	Down = 4,
	UR = 5,
	DR = 6,
	UL = 7,
	DL = 8
};

class Position
{
public:
	int x;
	int y;
	static int width;
	static int height;
	bool operator < (const Position b_) const;
	bool operator == (const Position b_) const;
};

string getStringForKey(const char* pKey, const string& defaultValue);
string getStringForKey(const char* pKey, const string& defaultValue, const string& filename);
string LS(string string_code);

typedef EventKeyboard::KeyCode Key_t;
char getCharForKeyCode(Key_t k, bool shift=false);

void recursivePause(Node*);
void recursiveResume(Node*);
void recursiveAction(Node* base, function<void(Node*)> f, bool runonbase=true);

#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)
#include "NDKHelper.h"
#endif

#endif