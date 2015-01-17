//
//  StorePurchasableAssets.h
//  GameOfDrones
//
//  Created by JAFARNEJAD Sasan on 6/1/14.
//
//

#ifndef __GameOfDrones__StorePurchasableAssets__
#define __GameOfDrones__StorePurchasableAssets__

#include "helpers.h"

#if IS_IOS
#include <iostream>

#include "CCIStoreAssets.h"
#include "cocos2d.h"

class StorePurchasableAssets: public soomla::CCIStoreAssets {
    cocos2d::Array *mCurrencies;
    cocos2d::Array *mGoods;
    cocos2d::Array *mCurrencyPacks;
    cocos2d::Array *mCategories;
    cocos2d::Array *mNonConsumableItems;
public:
    StorePurchasableAssets():
    mCurrencies(NULL),
    mGoods(NULL),
    mCurrencyPacks(NULL),
    mCategories(NULL),
    mNonConsumableItems(NULL)
    {}
    
    virtual ~StorePurchasableAssets();
    
    static StorePurchasableAssets *create();
    bool init();
    
    virtual int getVersion();
    
    virtual cocos2d::CCArray *getCurrencies();
    
    virtual cocos2d::CCArray *getGoods();
    
    virtual cocos2d::CCArray *getCurrencyPacks();
    
    virtual cocos2d::CCArray *getCategories();
    
    virtual cocos2d::CCArray *getNonConsumableItems();
};

#endif
#endif /* defined(__GameOfDrones__StorePurchasableAssets__) */
