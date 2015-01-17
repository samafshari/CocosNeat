#ifndef __VBar_H__
#define __VBar_H__

#include "helpers.h"

class VBar : public Node
{
private:
	bool _addToSelf;
protected:
	Node* bg;
	Menu* menu;
public:
    CREATE_FUNC(VBar);
    virtual bool init(void) override;
	virtual void addChild(Node* child) override;
};

class HBar : public Node
{
private:
	bool _addToSelf;
protected:
	Node* bg;
	Menu* menu, *menu2;
public:
	CREATE_FUNC(HBar);
	static HBar* CreateWithStandardButtons();
	virtual bool init(void) override;
	virtual void addChild(Node* child) override;
	virtual void addChild2(Node* child);
	void moveToTop();
};

#endif
