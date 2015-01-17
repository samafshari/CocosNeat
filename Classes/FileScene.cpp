#include "FileScene.h"
#include "helpers.h"
#include "GameSettings.h"
#include "Sidebar.h"

string FileScene::Caption = "Enter Filename";
string FileScene::Text = "";
function<void(string)> FileScene::Action = nullptr;

Scene* FileScene::createScene()
{
    auto scene = Scene::create();
	auto layer = FileScene::create();
    scene->addChild(layer);
    return scene;
}

bool FileScene::init()
{
    if ( !Layer::init() ) return false;
    
	auto keyboard = EventListenerKeyboard::create();
	keyboard->onKeyPressed = CC_CALLBACK_2(FileScene::keyPressed, this);
	keyboard->onKeyReleased = CC_CALLBACK_2(FileScene::keyReleased, this);
	getEventDispatcher()->addEventListenerWithSceneGraphPriority(keyboard, this);

	auto title = Label::createWithSystemFont(Caption, "Arial", 48.0f);
	title->setPosition(CP(0.5f, 0.6f));
	title->setColor(Color3B::WHITE);
	addChild(title);

	nameBox = EditBox::create(Size(PX(0.75f), PY(0.15f)), Scale9Sprite::createWithSpriteFrameName("editBox"));
	nameBox->setPosition(CP(0.5f, 0.45f));
	nameBox->setFont("Arial", 42.0f);
	//nameBox->setLabelAnchorPoint(Point(-0.1f, 0.5f));
	nameBox->setFontColor(Color3B::WHITE);
	addChild(nameBox);

	auto VBar = VBar::create();
	addChild(VBar, 1000);

	VBar->addChild(MAKEBUTTON("btnExitUp", "btnExitDn", FileScene::menuCloseCallback));
	VBar->addChild(MAKEBUTTON("Platformer_acceptup", "Platformer_acceptdn", FileScene::accept));

	_shift = false;
    return true;
}

void FileScene::keyReleased(EventKeyboard::KeyCode k, Event*)
{
	if (k == Key_t::KEY_SHIFT)
	{
		_shift = false;
		return;
	}
}

void FileScene::keyPressed(EventKeyboard::KeyCode k, Event*)
{
	if (k == Key_t::KEY_SHIFT)
	{
		_shift = true;
		return;
	}
	if (k == EventKeyboard::KeyCode::KEY_ESCAPE)
	{
		menuCloseCallback(this);
		return;
	}
	else if (k == EventKeyboard::KeyCode::KEY_ENTER)
	{
		accept(this);
		return;
	}

	if (k == Key_t::KEY_BACKSPACE)
	{
		string newt = nameBox->getText();
		if (newt.size()>1) newt.substr(0, newt.size() - 1);
		nameBox->setText(newt.c_str());
		return;
	}

	char c = getCharForKeyCode(k, _shift);
	if (c)
	{
		string newt = nameBox->getText();
		newt += nToString(c);
		nameBox->setText(newt.c_str());
	}
}

void FileScene::menuCloseCallback(Ref* pSender)
{
	Text = nToLower( nameBox->getText() );
    Director::getInstance()->popScene();
}

void FileScene::accept(Ref* pSender)
{
	Text = nToLower( nameBox->getText() );
	menuCloseCallback(pSender);
	if (Action) Action(Text);
}