#include "AppDelegate.h"
#include "NeatScene.h"
#include "Platformer.h"
#include "SimpleAudioEngine.h"
#include "Ooze.h"

#if (CC_TARGET_PLATFORM == CC_PLATFORM_IOS)
//For in-app-purchase
#include "CCEventHandler.h"
#endif

USING_NS_CC;
using namespace CocosDenshion;


#if defined(MINI_GAME)
string Ooze::GameName = "MiniGame";
#elif defined(NEAT_BUNDLE)
string Ooze::GameName = "NeatBundle";
#else
string Ooze::GameName = "GameOfDrones";
#endif

float ScaleFactor = 1.0f;
#if defined(LANDSCAPE) && defined(WINRT)
float DESIGN_X = 1920.0f;
float DESIGN_Y = 1080.0f;
#elif defined(LANDSCAPE)
float DESIGN_X = 960.0f;
float DESIGN_Y = 640.0f;
#else
float DESIGN_Y = 960.0f;
float DESIGN_X = 640.0f;
#endif

float VIEW_OFFSET_Y = 0;

float ASPECT_RATIO;
Size FRAME_SIZE;
int DPI;

#if !IS_IOS
float VIEW_WIDTH, VIEW_HEIGHT;
float WINDOW_WIDTH, WINDOW_HEIGHT;
#endif

#if defined(S_GAME)
#include "MiniMainMenu.h"
#include "DronesSplashScene.h"
#include "BundleGamesScene.h"
Scene* GetMainScene() {
	if (APPLICATION == Games::Drones)
		return DronesSplashScene::createScene();

	return BundleGamesScene::TryRunGame(APPLICATION, false);
}
#elif defined(NEAT_BUNDLE)
#include "BundleMainMenuScene.h"
Scene* GetMainScene() { return BundleMainMenu::scene(); }
#else
#include "DronesSplashScene.h"
Scene* GetMainScene() { return  DronesSplashScene::createScene(); }
#endif

AppDelegate::AppDelegate() {

}

AppDelegate::~AppDelegate() 
{
#if IS_IOS
	soomla::CCSoomla::sharedSoomla()->removeEventHandler(Purchase::handler);
#endif
}

//if you want a different context,just modify the value of glContextAttrs
//it will takes effect on all platforms
void AppDelegate::initGLContextAttrs()
{
    //set OpenGL context attributions,now can only set six attributions:
    //red,green,blue,alpha,depth,stencil
    GLContextAttrs glContextAttrs = {8, 8, 8, 8, 24, 8};

    GLView::setGLContextAttrs(glContextAttrs);
}

bool AppDelegate::applicationDidFinishLaunching() {
	srand((int) time(0));
	Ooze::InstanceId = rand();
	Ooze::LoadSettings();
	Ooze::InstantSend({ { "Event", "Hello" } });
	GameSettings::InitGameCenter(APPLICATION);
    GameSettings::FetchGameCenterData(APPLICATION);
    
	bool fullscreen = false;
    // initialize director
    auto director = Director::getInstance();
	auto glview = director->getOpenGLView();

    if(!glview) {
#if FULLSCREEN
		fullscreen = true;
#endif
#if (CC_TARGET_PLATFORM == CC_PLATFORM_WIN32)
		fullscreen = GameSettings::GetBool("Render_fullscreen", false);
#ifdef LANDSCAPE
		//DESIGN_X = GameSettings::GetFloat("Render_width", 1920.0f);
		//DESIGN_Y = GameSettings::GetFloat("Render_height", 1080.0f);
#else
		//DESIGN_X = GameSettings::GetFloat("Render_width", DESIGN_X);
		//DESIGN_Y = GameSettings::GetFloat("Render_height", DESIGN_Y);
#endif
#endif
		log(Ooze::GameName);
		glview = GLViewImpl::create(Ooze::GameName);
		glview->setFrameSize(DESIGN_X, DESIGN_Y);
			
        director->setOpenGLView(glview);
    }

    director->setDisplayStats(false);
    director->setAnimationInterval(1.0 / 60);


	Size frameSize = glview->getFrameSize();
	FRAME_SIZE = frameSize;

#if !IS_IOS
	VIEW_WIDTH = WINDOW_WIDTH = frameSize.width;
	VIEW_HEIGHT = WINDOW_HEIGHT = frameSize.height;
#endif

#ifdef LANDSCAPE
	ASPECT_RATIO = frameSize.width / frameSize.height;
	DESIGN_Y = (int) (0.5f + DESIGN_X / ASPECT_RATIO);
#else
	ASPECT_RATIO = frameSize.height / frameSize.width;
	DESIGN_Y = (int) (0.5f + DESIGN_X * ASPECT_RATIO);
#endif

	bool xd = false;
#if IS_IOS
	if (ASPECT_RATIO < 1.5f)
	{
		DESIGN_X = 1024.0f;
		DESIGN_Y = 768.0;
#ifndef LANDSCAPE
		swap(DESIGN_X, DESIGN_Y);
#endif
		if (frameSize.width >= 1080 || frameSize.height >= 1080)
		{
			xd = true;
		}

	}
#else
	if (DESIGN_X >= 1920 || DESIGN_Y >= 1920) xd = true;
#endif

#ifdef LANDSCAPE
	glview->setDesignResolutionSize(DESIGN_X, DESIGN_Y, ResolutionPolicy::FIXED_WIDTH);
#else
	glview->setDesignResolutionSize(DESIGN_X, DESIGN_Y, ResolutionPolicy::FIXED_HEIGHT);
#endif

	Size visibleSize = Director::getInstance()->getVisibleSize();
	DESIGN_X = visibleSize.width;
	DESIGN_Y = visibleSize.height;

#if (CC_TARGET_PLATFORM==CC_PLATFORM_ANDROID)
	xd = false;
#endif

	vector<string> watchFolders =
	{ "hd", "img", "maps", "particles", "shaders", "sounds", "meta" };

	if (xd)
	{
		watchFolders.insert(watchFolders.begin(), "xd");
		Director::getInstance()->setContentScaleFactor(2.0f);
	}
	else
	{
		watchFolders.push_back("xd");
		ScaleFactor = 0.5f;
	}

	if (APPLICATION == Games::Drones)
	{
		for (auto item : GameInfos) watch(item.first, watchFolders);
	}
	else watch(APPLICATION, watchFolders);
	loadContent();

	GameSettings::Set("Render_width", DESIGN_X);
	GameSettings::Set("Render_height", DESIGN_Y);
	GameSettings::Set("Render_fullscreen", fullscreen);

	GameSettings::LoadSettings();

	Scene* scene = NeatScene::createScene();
	
	Device::setKeepScreenOn(true);
	Device::setAccelerometerEnabled(true);
	Device::setAccelerometerInterval(0.1f);

	SimpleAudioEngine::getInstance()->setBackgroundMusicVolume(0.5f);

	log("View: " + nToString(VIEW_WIDTH) + "," + nToString(VIEW_HEIGHT));
	log("Window: " + nToString(WINDOW_WIDTH) + "," + nToString(WINDOW_HEIGHT));
	log("Design: " + nToString(DESIGN_X) + "," + nToString(DESIGN_Y));
	log("Frame Size: " + nToString(FRAME_SIZE.width) + "," + nToString(FRAME_SIZE.height));
	log("VCP(1): " + nToString(VPX(1)) + "," + nToString(VPY(1)));
    log("AR: " + nToString(ASPECT_RATIO));
#if defined(S_GAME)
	log("Game: " + GameInfos[APPLICATION].title);
#else
	log("No S_GAME");
#endif

#if PRO_VERSION
	GameSettings::ReportAchievement(ACHIEVEMENT_PROPLAYER);
#endif

    // run
    director->runWithScene(scene);

#ifndef NOBANNER
#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID) && defined(ADMOB)
	CreateBannerAd();
#endif

#if defined(BANNER_ON_BOTTOM)
	float VIEW_OFFSET_Y = (120.0f) / (VIEW_HEIGHT) * DESIGN_Y;
#else
#endif
#endif

    return true;
}

void AppDelegate::watch(Games game, vector<string> watchFolders)
{
	cocos2d::FileUtils::getInstance()->addSearchPath(GameInfos[game].title);
	for (auto f : watchFolders)
		cocos2d::FileUtils::getInstance()->addSearchPath(GameInfos[game].title + "/" + f);
	for (auto f : watchFolders)
		cocos2d::FileUtils::getInstance()->addSearchPath(f);
}

// This function will be called when the app is inactive. When comes a phone call,it's be invoked too
void AppDelegate::applicationDidEnterBackground() {
    Director::getInstance()->stopAnimation();

    // if you use SimpleAudioEngine, it must be pause
    // SimpleAudioEngine::getInstance()->pauseBackgroundMusic();
}

// this function will be called when the app is active again
void AppDelegate::applicationWillEnterForeground() {
    Director::getInstance()->startAnimation();

    // if you use SimpleAudioEngine, it must resume here
    // SimpleAudioEngine::getInstance()->resumeBackgroundMusic();
}


#define RAIDFRAME(sheet, name, x, y, w, h) FRAME(sheet, name, ((x)+2048), ((y)+2048), (w), (h))
void AppDelegate::loadContent()
{
	log("Loading content...");
	LOADATLAS(sheet1, "nats1.png");
	LOADATLAS(sheet2, "nats2.png");

	FRAME(sheet1, "btnSoundUp", 128, 0, 128, 128);
	FRAME(sheet1, "btnSoundDn", 128, 0, 128, 128);

	FRAME(sheet1, "btnMuteUp", 256, 0, 128, 128);
	FRAME(sheet1, "btnMuteDn", 384, 0, 128, 128);

	FRAME(sheet1, "btnExitUp", 512, 0, 128, 126);
	FRAME(sheet1, "btnExitDn", 640, 0, 128, 126);

	FRAME(sheet1, "btnRefUp", 768, 0, 128, 128);
	FRAME(sheet1, "btnRefDn", 896, 0, 128, 128);

	FRAME(sheet1, "btnBackUp", 1024, 0, 128, 128);
	FRAME(sheet1, "btnBackDn", 1152, 0, 128, 128);

	FRAME(sheet1, "btnNextUp", 1280, 0, 128, 128);
	FRAME(sheet1, "btnNextDn", 1408, 0, 128, 128);

	FRAME(sheet1, "btnMenuUp", 1536, 0, 128, 128);
	FRAME(sheet1, "btnMenuDn", 1664, 0, 128, 128);

	FRAME(sheet1, "btnHelpUp", 128, 384, 128, 128);
	FRAME(sheet1, "btnHelpDn", 256, 384, 128, 128);

	FRAME(sheet1, "btnArrowsUp", 1792, 384, 128, 128);
	FRAME(sheet1, "btnArrowsDn", 1920, 384, 128, 128);
	FRAME(sheet1, "btnPilotUp", 1792, 512, 128, 128);
	FRAME(sheet1, "btnPilotDn", 1920, 512, 128, 128);

	FRAME(sheet1, "btnBoxUp", 0, 512, 128, 128);
	FRAME(sheet1, "btnBoxDn", 128, 512, 128, 128);

	FRAME(sheet1, "btnCoverUp", 384, 384, 128, 128);
	FRAME(sheet1, "btnCoverDn", 512, 384, 128, 128);

	FRAME(sheet1, "wallReflection", 1792, 0, 128, 128);
	FRAME(sheet1, "wallShadow", 1920, 0, 128, 128);

	FRAME(sheet1, "ghmonstermovable", 0, 128, 128, 128);

	FRAME(sheet1, "gem", 256, 128, 128, 128);
	FRAME(sheet1, "wallGradient", 384, 128, 128, 128);

	FRAME(sheet1, "bluedoor", 512, 128, 128, 128);
	FRAME(sheet1, "star", 770, 132, 122, 124);
	FRAME(sheet1, "dronesGoal", 770, 132, 122, 124);
	FRAME(sheet1, "route", 768, 256, 126, 128);

	FRAME(sheet1, "manUp1", 896, 128, 128, 128);
	FRAME(sheet1, "manUp2", 1024, 128, 128, 128);
	FRAME(sheet1, "manUp3", 1152, 128, 128, 128);
	FRAME(sheet1, "manUp4", 1280, 128, 128, 128);

	FRAME(sheet1, "manDn1", 1408, 128, 128, 128);
	FRAME(sheet1, "manDn2", 1536, 128, 128, 128);
	FRAME(sheet1, "manDn3", 1664, 128, 128, 128);
	FRAME(sheet1, "manDn4", 1792, 128, 128, 128);

	FRAME(sheet1, "manRt1", 896, 256, 128, 128);
	FRAME(sheet1, "manRt2", 1024, 256, 128, 128);
	FRAME(sheet1, "manRt3", 1152, 256, 128, 128);
	FRAME(sheet1, "manRt4", 1280, 256, 128, 128);

	FRAME(sheet1, "manLt1", 1408, 256, 128, 128);
	FRAME(sheet1, "manLt2", 1536, 256, 128, 128);
	FRAME(sheet1, "manLt3", 1664, 256, 128, 128);
	FRAME(sheet1, "manLt4", 1792, 256, 128, 128);

	FRAME(sheet1, "ghmonster", 0, 256, 128, 128);

	FRAME(sheet1, "doorBlue", 128, 256, 128, 128);
	FRAME(sheet1, "box", 640, 128, 128, 128);
	FRAME(sheet1, "sd", 256, 256, 128, 128);
	FRAME(sheet1, "usb", 384, 256, 128, 128);
	FRAME(sheet1, "intel", 512, 256, 128, 128);
	FRAME(sheet1, "lolli", 640, 384, 128, 128);

	FRAME(sheet1, "repeat", 256, 512, 128, 128);
	FRAME(sheet1, "button", 512, 512, 128, 128);
	FRAME(sheet1, "door", 384, 512, 128, 128);
	FRAME(sheet1, "condition", 640, 512, 128, 128);
	FRAME(sheet1, "once", 768, 512, 128, 128);
	FRAME(sheet1, "finish", 1664, 384, 128, 128);
	FRAME(sheet1, "condition1", 1664, 512, 128, 128);
	FRAME(sheet1, "destination", 0, 640, 128, 128);
	FRAME(sheet1, "teleport", 128, 640, 128, 128);
	FRAME(sheet1, "plate", 384, 640, 128, 128);
	FRAME(sheet1, "player", 896, 128, 128, 128);
	FRAME(sheet1, "hazard", 1920, 128, 128, 128);
	FRAME(sheet1, "solid", 1180, 640, 128, 128);

	FRAME(sheet1, "choosetxt", 1394, 790, 652, 116);
	FRAME(sheet1, "choosetxt_demo", 1394, 790, 652, 116);
	FRAME(sheet1, "storelogo", 1050, 2064, 804, 229);
	FRAME(sheet1, "blackholebig", 2944, 28, 800, 800);
	FRAME(sheet1, "gamelogotext", 16, 1482, 788, 194);
#ifdef NEAT_BUNDLE
	FRAME(sheet1, "neatlogo", 2189, 892, 1025, 426);
	FRAME(sheet1, "gamelogo", 3214, 880, 685, 668);
#else
	FRAME(sheet1, "gamelogo", 2110, 28, 800, 800);
	FRAME(sheet1, "neatlogo", 2667, 2886, 966, 176);
#endif
	FRAME(sheet1, "facebook", 1310, 640, 736, 150);
	FRAME(sheet1, "facebookshare", 3248, 2551, 504, 162);
	FRAME(sheet1, "facebooksharehighscore", 3248, 2713, 789, 164);

	FRAME(sheet1, "circle", 896, 384, 256, 256);
	FRAME(sheet1, "circleDn", 1152, 384, 256, 256);
	FRAME(sheet1, "circleLk", 1408, 384, 256, 256);

	FRAME(sheet1, "arrRt", 648, 264, 50, 32);
	FRAME(sheet1, "arrLt", 704, 264, 50, 32);
	FRAME(sheet1, "arrUp", 656, 306, 32, 50);
	FRAME(sheet1, "arrDn", 716, 306, 32, 50);
	FRAME(sheet1, "arrSwipe", 1930, 296, 112, 50);

	FRAME(sheet1, "arrPanel", 1746, 1206, 300, 300);
	FRAME(sheet1, "arrBall", 642, 640, 164, 164);
	FRAME(sheet1, "arrMoveBtn", 768, 384, 128, 128);
	FRAME(sheet1, "arrPiece", 1746, 1506, 300, 300);
	FRAME(sheet1, "arrArrs", 1746, 906, 300, 300);

	FRAME(sheet1, "eggplant", 768, 256, 128, 128);
	FRAME(sheet1, "banana", 128, 384, 128, 128);
	FRAME(sheet1, "orange", 256, 384, 128, 128);
	FRAME(sheet1, "lemon", 384, 384, 128, 128);
	FRAME(sheet1, "apple", 510, 384, 128, 128);

	FRAME(sheet1, "btnBackBlackUp", 830, 646, 164, 164);
	FRAME(sheet1, "btnBackBlackDn", 998, 646, 164, 164);

	FRAME(sheet1, "credits1", 0, 814, 688, 120);
	FRAME(sheet1, "credits11", 970, 934, 574, 106);
	FRAME(sheet1, "credits2", 688, 814, 614, 120);
	FRAME(sheet1, "credits21", 970, 1040, 676, 108);

	FRAME(sheet1, "robot", 0, 384, 128, 128);
	FRAME(sheet1, "robotf", 0, 128, 128, 128);

	FRAME(sheet1, "btnDn", 22, 1090, 778, 176);
	FRAME(sheet1, "btnUp", 22, 1280, 778, 176);
	FRAME(sheet1, "btnGreenUp", 828, 1362, 778, 176);
	FRAME(sheet1, "btnGreenDn", 828, 1172, 778, 176);
	FRAME(sheet1, "btnBlueUp", 532, 3906, 778, 176);
	FRAME(sheet1, "btnBlueDn", 532, 3716, 778, 176);
	FRAME(sheet1, "btnRestore", 3065, 2910, 540, 165);
	FRAME(sheet1, "btnBlueSmall", 532, 3522, 380, 176);
	FRAME(sheet1, "btnPurpleSmall", 930, 3522, 380, 176);

	FRAME(sheet1, "btnMesh", 544, 1792, 256, 256);
	FRAME(sheet1, "btnSnake", 800, 1792, 256, 256);
	FRAME(sheet1, "btnTree", 1056, 1792, 256, 256);
	FRAME(sheet1, "btnNaturals", 1312, 1792, 256, 256);
	FRAME(sheet1, "btnRaid", 1568, 1792, 256, 256);
	FRAME(sheet1, "btnFall", 1824, 1792, 256, 256);
	FRAME(sheet1, "btnLock", 3840, 208, 256, 256);

	FRAME(sheet1, "meshbig", 2121, 1508, 512, 512);
	FRAME(sheet1, "snakebig", 2643, 1507, 512, 512);
	FRAME(sheet1, "treebig", 0, 2576, 512, 512);
	FRAME(sheet1, "raidbig", 9, 2064, 512, 512);
	FRAME(sheet1, "fallbig", 538, 2064, 512, 512);
	FRAME(sheet1, "melodiebig", 1065, 2480, 512, 512);

	FRAME(sheet1, "bw0", 1664, 934, 128, 128);
	FRAME(sheet1, "bw1", 1792, 934, 128, 128);
	FRAME(sheet1, "bw2", 1920, 934, 128, 128);
	FRAME(sheet1, "bw3", 1664, 1062, 128, 128);
	FRAME(sheet1, "bw4", 1792, 1062, 128, 128);
	FRAME(sheet1, "bw5", 1920, 1062, 128, 128);
	FRAME(sheet1, "bw6", 1664, 1190, 128, 128);
	FRAME(sheet1, "bw7", 1792, 1190, 128, 128);

	FRAME(sheet1, "gemholder", 4, 1692, 540, 165);
	FRAME(sheet1, "gemholdertimer", 1568, 1599, 540, 165);
	FRAME(sheet1, "gemholdertimersuper", 1065, 2307, 540, 165);
	FRAME(sheet1, "btnMineUp", 1663, 1325, 540, 165);
	FRAME(sheet1, "btnMineDn", 2203, 1325, 540, 165);

	//mesh
	FRAME(sheet1, "vertex", 248, 1860, 128, 128);
	FRAME(sheet1, "ring", 3809, 1799, 256, 256);

	//tree
	FRAME(sheet1, "manLeft", 1106, 1560, 144, 198);
	FRAME(sheet1, "manRight", 1256, 1560, 142, 198);
	FRAME(sheet1, "manUp", 990, 1560, 108, 204);
	FRAME(sheet1, "tree0", 834, 1574, 150, 170);
	FRAME(sheet1, "tree1", 1406, 1576, 140, 168);

	//fall
	FRAME(sheet1, "ball", 3918, 28, 160, 166);

	//rrx
	FRAME(sheet1, "btnLaserUp", 2800, 3172, 160, 160);
	FRAME(sheet1, "btnRocketUp", 2800, 3354, 160, 160);
	FRAME(sheet1, "btnGunUp", 2800, 3542, 160, 160);

	FRAME(sheet1, "btnLaserDn", 2054, 3712, 160, 160);
	FRAME(sheet1, "btnRocketDn", 2222, 3716, 160, 160);
	FRAME(sheet1, "btnGunDn", 2394, 3716, 160, 160);

	FRAME(sheet1, "btnLaserOn", 2054, 3526, 160, 160);
	FRAME(sheet1, "btnRocketOn", 2222, 3530, 160, 160);
	FRAME(sheet1, "btnGunOn", 2394, 3530, 160, 160);

	RAIDFRAME(sheet1, "bridge", 1160, 144, 640, 352);
	RAIDFRAME(sheet1, "dashboard", 10, 396, 600, 160);
	RAIDFRAME(sheet1, "heli1", 650, 390, 226, 128);
	RAIDFRAME(sheet1, "heli2", 924, 390, 226, 128);
	RAIDFRAME(sheet1, "jet", 1000, 532, 144, 80);
	RAIDFRAME(sheet1, "ship", 22, 582, 400, 142);
	RAIDFRAME(sheet1, "tree", 624, 570, 196, 138);
	RAIDFRAME(sheet1, "house", 1662, 932, 370, 136);
	RAIDFRAME(sheet1, "raider", 1748, 1094, 210, 224);
	RAIDFRAME(sheet1, "raiderrt", 1780, 1330, 154, 228);
	RAIDFRAME(sheet1, "road", 64, 1094, 640, 352);
	RAIDFRAME(sheet1, "fuel", 1422, 1168, 184, 410);

	RAIDFRAME(sheet1, "rocket", 512, 626, 16, 64);
	RAIDFRAME(sheet1, "fatrocket", 548, 628, 22, 64);
	RAIDFRAME(sheet1, "laser", 582, 628, 14, 60);
	RAIDFRAME(sheet1, "bullet", 478, 628, 12, 30);

	RAIDFRAME(sheet1, "speedbar", 976, 1160, 256, 600);

	//Platformer
	FRAME(sheet1, "Platformer_wall", 394, 904, 128, 128);
	FRAME(sheet1, "Platformer_spike", 131, 772, 128, 128);
	FRAME(sheet1, "Platformer_ball", 0, 905, 128, 128);
	FRAME(sheet1, "Platformer_ball_onair", 131, 905, 128, 128);
	FRAME(sheet1, "Platformer_glass", 394, 773, 128, 128);
	FRAME(sheet1, "Platformer_bgstar", 644, 808, 92, 92);
	FRAME(sheet1, "Platformer_water", 262, 904, 128, 128);

	FRAME(sheet1, "Platformer_pauseup", 744, 817, 128, 128);
	FRAME(sheet1, "Platformer_pausedn", 877, 817, 128, 128);
	FRAME(sheet1, "Platformer_playup", 1009, 817, 128, 128);
	FRAME(sheet1, "Platformer_playdn", 1144, 817, 128, 128);
	FRAME(sheet1, "Platformer_stopup", 1278, 797, 128, 128);
	FRAME(sheet1, "Platformer_stopdn", 1412, 797, 128, 128);
	FRAME(sheet1, "Platformer_itemsup", 1546, 797, 128, 128);
	FRAME(sheet1, "Platformer_itemsdn", 1681, 797, 128, 128);
	FRAME(sheet1, "Platformer_refup", 768, 0, 128, 128);
	FRAME(sheet1, "Platformer_refdn", 896, 0, 128, 128);
	FRAME(sheet1, "Platformer_plusup", 1813, 797, 128, 128);
	FRAME(sheet1, "Platformer_plusdn", 1945, 797, 128, 128);
	FRAME(sheet1, "Platformer_saveup", 744, 950, 128, 128);
	FRAME(sheet1, "Platformer_savedn", 877, 950, 128, 128);
	FRAME(sheet1, "Platformer_moveup", 1009, 951, 128, 128);
	FRAME(sheet1, "Platformer_movedn", 1144, 950, 128, 128);
	FRAME(sheet1, "Platformer_acceptup", 1278, 951, 128, 128);
	FRAME(sheet1, "Platformer_acceptdn", 1412, 951, 128, 128);
	FRAME(sheet1, "Platformer_godup", 2055, 934, 128, 128);
	FRAME(sheet1, "Platformer_goddn", 2055, 1069, 128, 128);
	FRAME(sheet1, "editBox", 22, 1280, 778, 176);

	FRAME(sheet1, "heart_on_small", 3844, 725, 80, 80);
	FRAME(sheet1, "heart_off_small", 3928, 725, 80, 80);

	FRAME(sheet1, "heart_on", 0, 3098, 512, 449);
	FRAME(sheet1, "heart_off", 0, 3555, 512, 449);
	FRAME(sheet1, "bigcircle", 538, 2589, 512, 449);

	//Blob
	FRAME(sheet1, "blob_m_happy", 3782, 1548, 125, 78);
	FRAME(sheet1, "blob_m_shock", 3794, 1626, 85, 75);
	FRAME(sheet1, "blob_m_bomb", 3158, 1871, 125, 78);
	FRAME(sheet1, "blob_eye", 3796, 1703, 74, 77);
	FRAME(sheet1, "blob_face", 3477, 1551, 315, 315);
	FRAME(sheet1, "blob_bomb", 3158, 1551, 315, 315);
	FRAME(sheet1, "blob_alpha", 3473, 1871, 315, 315);

	FRAME(sheet1, "cloud1", 1338, 3639, 638, 435);
	FRAME(sheet1, "cloud2", 538, 3063, 513, 354);

	FRAME(sheet1, "btnfacebookup", 2743, 1325, 128, 128);
	FRAME(sheet1, "btnfacebookdn", 2871, 1325, 128, 128);
	FRAME(sheet1, "btncopyrightup", 3968, 832, 128, 128);
	FRAME(sheet1, "btncopyrightdn", 3968, 960, 128, 128);

	FRAME(sheet1, "btnMusicOffUp", 3968, 1089, 128, 128);
	FRAME(sheet1, "btnMusicOffDn", 3968, 1089, 128, 128);
	FRAME(sheet1, "btnMusicOnUp", 3968, 1217, 128, 128);
	FRAME(sheet1, "btnMusicOnDn", 3968, 1217, 128, 128);

	FRAME(sheet1, "btnDownArrowUp", 3968, 1345, 128, 128);
	FRAME(sheet1, "btnDownArrowDn", 3968, 1473, 128, 128);

	//Sheet2
	FRAME(sheet2, "circle_large", 0, 0, 490, 490);
	FRAME(sheet2, "circle_large_half", 0, 0, 490, 490 / 2);
	FRAME(sheet2, "circle_line", 0, 496, 490, 38);
	FRAME(sheet2, "circle_medium", 490, 0, 160, 160);
	FRAME(sheet2, "notif_exclamation", 490, 160, 126, 124);
	//FRAME(sheet2, "contrast_block", 756, 0, 240, 240);
    FRAME(sheet2, "contrast_block", 490, 0, 160, 160);
    
#if !defined(LIGHT_THEME)
	FRAME(sheet2, "settings_icon", 490, 284, 110, 110);
	FRAME(sheet2, "like_icon", 650, 0, 101, 99);
	FRAME(sheet2, "stats_icon", 650, 99, 106, 73);
#else
	FRAME(sheet2, "settings_icon", 616, 172, 110, 110);
	FRAME(sheet2, "like_icon", 600, 284, 101, 99);
	FRAME(sheet2, "stats_icon", 490, 394, 106, 73);
#endif

	FRAME(sheet2, "arrow_next_up", 726, 240, 128, 128);
	FRAME(sheet2, "arrow_next_dn", 854, 240, 128, 128);

	FRAME(sheet2, "btnBlob", 0, 550, 256, 256);
	FRAME(sheet2, "btnContrast", 256, 550, 256, 256);
	FRAME(sheet2, "btnMesh2", 512, 490, 256, 256);

	//Matryoshka
	if (APPLICATION == Games::Matryoshka)
	{
		LOADATLAS(matr, "matryoshka.png");
		FRAME(matr, "matr0", 0, 0, 256, 256);
		FRAME(matr, "matr1", 256, 0, 256, 256);
		FRAME(matr, "matr2", 512, 0, 256, 256);
		FRAME(matr, "matr3", 768, 0, 256, 256);
		FRAME(matr, "matr4", 0, 256, 256, 256);
		FRAME(matr, "matr5", 256, 256, 256, 256);
		FRAME(matr, "matr6", 512, 256, 256, 256);
		FRAME(matr, "matr7", 768, 256, 256, 256);
		FRAME(matr, "matr8", 0, 512, 256, 256);
		FRAME(matr, "matr9", 256, 512, 256, 256);
	}


#if !defined(BURGERMAN)
	if (APPLICATION == Games::Drones)
	{
		LOADATLAS(bg0, "bg0.png");
		FRAME(bg0, "bg", 2048, 2048, 2048, 2048);
		FRAME(bg0, "bg3", 2048, 2048, 2048, 2048);
		FRAME(bg0, "bg0", 0, 0, 2048, 2048);
		FRAME(bg0, "bg1", 2048, 0, 2048, 2048);
		FRAME(bg0, "bg2", 0, 2048, 2048, 2048);
	}
#else
	LOADATLAS(burger, "burgerman.png");
	FRAME(burger, "dronesGoal", 0, 0, 128, 128);
	FRAME(burger, "robot", 256, 0, 128, 128);
	FRAME(burger, "robotf", 384, 0, 128, 128);
	FRAME(burger, "gamelogo", 128, 211, 384, 301);
	FRAME(burger, "arrSwipe", 10, 424, 112, 50);
#endif

	//Contrast
	FRAME(sheet1, "contrastbig", 1065, 2992, 512, 512);
	
	if (APPLICATION == Games::Contrast)
	{
		LOADATLAS(contrast, "contrastic_sheet.fw.png");
		FRAME(contrast, "c_pink", 0, 0, 410, 410);
		FRAME(contrast, "c_blue", 410, 0, 410, 410);
		FRAME(contrast, "c_gray", 820, 0, 410, 410);
		FRAME(contrast, "lc_pink", 1230, 0, 577, 115);
		FRAME(contrast, "lc_blue", 1230, 115, 577, 115);
		FRAME(contrast, "lc_gray", 1230, 230, 577, 115);
	}
}