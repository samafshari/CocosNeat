#ifndef __TOPANCHOR_H__
#define __TOPANCHOR_H__

#include "helpers.h"
USING_NS_CC;

class TopAnchor : public Node
{
public:
    float oldViewHeight;
    CREATE_FUNC(TopAnchor);
    bool init(void);
    
    //std::vector<Node*> objects;
    //void addObject(Node*);
    void update(float);
};

#endif
