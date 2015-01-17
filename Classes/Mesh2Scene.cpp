#include "Mesh2Scene.h"
#include "ExtraGameOverScene.h"
#include "BackgroundWidget.h"
#include "Ooze.h"
#include "Mesh2Graph.h"
#include "Widgets.h"
#include "Sidebar.h"

namespace Mesh2
{
	Scene* Mesh2Scene::scene()
	{
		Ooze::InstantSend({ { "Event", "GameStart" }, { "Game", "Mesh2" } });
		GameSettings::OozeGameStats();
		Scene *scene = Scene::create();
		Mesh2Scene *layer = Mesh2Scene::create();
		scene->addChild(layer);
		return scene;
	}

	bool Mesh2Scene::init()
	{
		if (!Layer::init()) return false;

		auto bgcolor = LayerColor::create(GameInfos[Games::Mesh2].color, DESIGN_X, DESIGN_Y);
		addChild(bgcolor);

		addChild(TiledBackground::create("mesh2_pattern.png"));
		//addChild(BackgroundWidget::createWithoutBackground());

		float menuY = 0.95f;
		auto hooker = TopAnchor::create();
		addChild(hooker, 1);
		pMenu = Menu::create();
		pMenu->setPosition(VCP(0.0f, menuY));
		hooker->addChild(pMenu, 1);

		Mesh2GraphNode* graph = Mesh2GraphNode::create({ 5, 6 }, 1.0f);
		graph->setPosition(CP(0.1f, 0.2f));
		graph->end = CC_CALLBACK_0(Mesh2Scene::winGame, this);
		addChild(graph);

		auto sidebar = HBar::CreateWithStandardButtons();
		addChild(sidebar);
		sidebar->addChild(MakeButton("Platformer_refup", "Platformer_refdn", " ",
			10.0f, [](Ref*) {
			Director::getInstance()->replaceScene(Mesh2Scene::scene());
		}));

		auto keyboard = EventListenerKeyboard::create();
		keyboard->onKeyReleased = CC_CALLBACK_2(Mesh2Scene::onKeyReleased, this);
		getEventDispatcher()->addEventListenerWithSceneGraphPriority(keyboard, this);

		auto touch = EventListenerTouchOneByOne::create();
		touch->onTouchBegan = CC_CALLBACK_2(Mesh2Scene::onTouchBegan, this);
		touch->onTouchMoved = CC_CALLBACK_2(Mesh2Scene::onTouchMoved, this);
		touch->onTouchEnded = CC_CALLBACK_2(Mesh2Scene::onTouchEnded, this);
		touch->onTouchCancelled = CC_CALLBACK_2(Mesh2Scene::onTouchEnded, this);
		getEventDispatcher()->addEventListenerWithSceneGraphPriority(touch, this);

		//addChild(ModalDialog::create("hi"));

		return true;
	}

	void Mesh2Scene::btnBackPressed(Ref*)
	{
		loseGame(true);
	}

	void Mesh2Scene::onKeyReleased(Key_t keyCode, Event* event)
	{
		if (keyCode == Key_t::KEY_ESCAPE)
			btnBackPressed(this);
		if (keyCode == Key_t::KEY_G)
		{
			//Mesh2Graph graph = Mesh2Graph::Create({ 4, 4 });
			//log(graph.ToString());

		}
	}

	bool Mesh2Scene::onTouchBegan(Touch* touch, Event*)
	{
		return true;
	}

	void Mesh2Scene::onTouchMoved(Touch* touch, Event*)
	{

	}

	void Mesh2Scene::onTouchEnded(Touch* touch, Event*)
	{

	}

	void Mesh2Scene::winGame()
	{

	}

	void Mesh2Scene::loseGame(bool backPressed)
	{
	}
}