#include "TopAnchor.h"

bool TopAnchor::init()
{
    if (!Node::init()) return false;
    oldViewHeight = VIEW_HEIGHT;
    
    scheduleUpdate();
    return true;
}
/*
void TopAnchor::addObject(Node* n)
{
    objects.push_back(n);
}*/

void TopAnchor::update(float)
{
    if (oldViewHeight != VIEW_HEIGHT)
    {
        float dy = VIEW_HEIGHT - oldViewHeight;
        runAction(MoveBy::create(0.25f, Point(0, dy)));
        oldViewHeight = VIEW_HEIGHT;
    }
}/*
        for (auto o : objects)
        {
            auto family = o->getParent()->getChildren();
            if (family.find(o) == family.end())
            {
                log("Orphan.");
                return;
            }
            float dy = VIEW_HEIGHT - oldViewHeight;
            o->runAction(MoveBy::create(0.25f, Point(0, dy)));
            //o->runAction(MoveTo::create(0.25f, Point(o->getPositionX(), dy)));
        }
        oldViewHeight = VIEW_HEIGHT;
    }
}*/