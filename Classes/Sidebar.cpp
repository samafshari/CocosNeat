#include "Sidebar.h"
#include "Widgets.h"
#include "CreditsScene.h"

//////////////// VBar
bool VBar::init()
{
    if (!Node::init()) return false;
	_addToSelf = true;

	const float width = PX(0.07f);
	setPosition(Point(DESIGN_X - width, 0));
	setContentSize(Size(width, DESIGN_Y));
	setAnchorPoint(Point(0, 0));

	bg = LayerColor::create(Color4B(10,10,10,240), width, DESIGN_Y);
	addChild(bg);

	menu = Menu::create();
	menu->setPosition(Point::ZERO);
	addChild(menu);

	_addToSelf = false;
	return true;
}

void VBar::addChild(Node* child)
{
	if (_addToSelf)
	{
		Node::addChild(child);
		return;
	}

	const float startOffset = PY(0.05f);
	const float spacing = PY(0.02f);
	const float scale = 0.6f;

	auto mysize = getContentSize();
	float y = mysize.height - startOffset;

	for (auto node : menu->getChildren())
		y -= node->getContentSize().height + spacing;

	menu->addChild(child);
	auto cz = child->getBoundingBox().size;
	Size newSize = mysize;

	newSize.height = cz.height / cz.width * newSize.width;
	child->setScaleX(scale * newSize.width / cz.width);
	child->setScaleY(scale * newSize.height / cz.height);
	child->setPosition(Point(mysize.width * 0.5f, y));
	child->setAnchorPoint(Point(0.5f, 1));
}


///////////// HBar
bool HBar::init()
{
	if (!Node::init()) return false;
	_addToSelf = true;

	const float height = PY(0.1f);
	setPosition(Point(0, 0));
	setContentSize(Size(DESIGN_X, height));
	setAnchorPoint(Point(0, 0));

	bg = LayerColor::create(Color4B(10, 10, 10, 240), DESIGN_X, height);
	addChild(bg);

	menu = Menu::create();
	menu->setPosition(Point::ZERO);
	addChild(menu);

	menu2 = Menu::create();
	menu2->setPosition(Point::ZERO);
	addChild(menu2);

	_addToSelf = false;
	return true;
}

void HBar::addChild(Node* child)
{
	if (_addToSelf)
	{
		Node::addChild(child);
		return;
	}

	const float startOffset = PX(0.05f);
	const float spacing = PX(0.02f);
	const float scale = 0.6f;

	auto mysize = getContentSize();
	float y = startOffset;

	for (auto node : menu->getChildren())
		y += node->getContentSize().width + spacing;

	menu->addChild(child);
	auto cz = child->getBoundingBox().size;
	Size newSize = mysize;

	newSize.width = cz.width / cz.height * newSize.height;
	child->setScaleX(scale * newSize.width / cz.width);
	child->setScaleY(scale * newSize.height / cz.height);
	child->setPosition(Point(y, mysize.height * 0.5f));
	child->setAnchorPoint(Point(0, 0.5f));
}


void HBar::addChild2(Node* child)
{
	if (_addToSelf)
	{
		Node::addChild(child);
		return;
	}

	const float startOffset = PX(0.05f);
	const float spacing = PX(0.02f);
	const float scale = 0.6f;

	auto mysize = getContentSize();
	float y = PX(1.0f) - startOffset;

	for (auto node : menu2->getChildren())
		y -= node->getContentSize().width + spacing;

	menu2->addChild(child);
	auto cz = child->getBoundingBox().size;
	Size newSize = mysize;

	newSize.width = cz.width / cz.height * newSize.height;
	child->setScaleX(scale * newSize.width / cz.width);
	child->setScaleY(scale * newSize.height / cz.height);
	child->setPosition(Point(y, mysize.height * 0.5f));
	child->setAnchorPoint(Point(1.0f, 0.5f));
}

HBar* HBar::CreateWithStandardButtons()
{
	auto HBar = HBar::create();

	HBar->addChild(MusicButton::create());
	HBar->addChild2(SoundButton::create());

	auto btnFacebook = MakeButton("btnfacebookup", "btnfacebookdn", " ", 10, [](Ref*) {	OpenFacebookPage();	});
	HBar->addChild(btnFacebook);
	auto btnRate = MakeButton("Platformer_itemsup", "Platformer_itemsdn", " ", 10.0f, [](Ref*) { OpenStorePage(); });
	HBar->addChild2(btnRate);

	auto btnCopy = MakeButton("btncopyrightup", "btncopyrightdn", " ", 10, [](Ref*) {	CreditsScene::ShowCredits("dronescredits.txt");	});
	HBar->addChild2(btnCopy);

	return HBar;
}

void HBar::moveToTop()
{
	setAnchorPoint(Point(0, 1));
	setPositionY(VPY(1.0f));
}