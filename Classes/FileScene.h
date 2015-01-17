#ifndef __FILE_SCENE_H__
#define __FILE_SCENE_H__

#include "helpers.h"
#include "cocos-ext.h"
//#include "CCEditBox.h"
//#include "extensions/GUI/CCControlExtension/CCEditBox.h"
//#include "extensions/GUI/CCEditBox/CCEditBox.h"

using namespace cocos2d::extension;

class FileScene : public cocos2d::Layer
{
public:
	EditBox* nameBox;
	static string Caption;
	static string Text;
	static function<void(string)> Action;
    static cocos2d::Scene* createScene();
    virtual bool init(); 
	
	CREATE_FUNC(FileScene);
private:
	bool _shift;
	void keyPressed(EventKeyboard::KeyCode k, Event*);
	void keyReleased(EventKeyboard::KeyCode k, Event*);
	void menuCloseCallback(Ref* pSender);
	void accept(Ref*);
};

#endif
