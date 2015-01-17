#ifndef __GEMSLAYER_H__
#define __GEMSLAYER_H__

#include "helpers.h"

class GemsLayer : public Node
{
public:
    virtual bool init();  
	CREATE_FUNC(GemsLayer);
    int lastGems;
    Menu* menu;
    static Sprite* createInside();
    void putStuffInMenu();
    void pressed(Ref*);
    void update(float) override;
    
};

class PauseLayer : public Layer
{
public:
    CREATE_FUNC(PauseLayer);
    Node* parentScene;
	function<void()> resumefunc;
	function<void()> stopfunc;

    static void Pause(Node* parent, 
		function<void()> _resumefunc = nullptr,
		function<void()> _stopfunc = nullptr);
    
    virtual bool init();
    void postInit();
    bool touched(Touch*, Event*);
	void touchEnded(Touch*, Event*);
	void btnBackPressed(Ref*);
	void btnExitPressed(Ref*);
	void onKeyReleased(EventKeyboard::KeyCode keyCode, Event* event);
};

class GemTimerLayer : public Node
{
public:
    int lastTime;
    virtual bool init();
    CREATE_FUNC(GemTimerLayer);
    Sprite* up;
    Sprite* dn;
    static Sprite* createInside();
    void pressed(Ref*);
    void updateText(float);
};

class FacebookShareButton : public Node
{
public:
	bool isHighscore;
	int score;
	Games game;
	static FacebookShareButton* create(bool highscore, int, Games);
	virtual bool init(); 
	static Sprite* createInside(bool highscore, int);

	void pressed(Ref*);
};

class Vignette : public Layer
{
public:
	static Vignette* create();
};

class SoundButton : public MenuItemSprite
{
public:
	static SoundButton* create();
	void btnSoundPressed(Ref*);
};

class MusicButton : public MenuItemSprite
{
public:
	static MusicButton* create();
	void btnMusicPressed(Ref*);
};

class TiledBackground : public Layer
{
public:
	static Sprite* create(string filename);
};

class ModalDialog : public Layer
{
public:
	EventListenerTouchOneByOne* listener;

	static ModalDialog* create(string text);
	bool init(string text);

	bool onTouchBegan(Touch*, Event*);
	//void onTouchEnded(Touch*, Event*);

	function<void()> btnOkPressed;
};
#endif
