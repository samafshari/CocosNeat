//
//  StorePurchasableAssets.cpp
//  GameOfDrones
//
//  Created by JAFARNEJAD Sasan on 6/1/14.
//
//

#include "StorePurchasableAssets.h"

#if IS_IOS
#include "CCVirtualCurrency.h"
#include "CCVirtualCurrencyPack.h"
#include "CCVirtualGood.h"
#include "CCSingleUseVG.h"
#include "CCPurchaseWithMarket.h"
#include "CCPurchaseWithVirtualItem.h"
#include "CCVirtualCategory.h"
#include "CCNonConsumableItem.h"
#include "CCUpgradeVG.h"
#include "CCSingleUsePackVG.h"
#include "CCEquippableVG.h"
#include "Purchase.h"

USING_NS_CC;
using namespace soomla;



StorePurchasableAssets *StorePurchasableAssets::create() {
    StorePurchasableAssets *ret = new StorePurchasableAssets();
    ret->autorelease();
    ret->init();
    
    return ret;
}

bool StorePurchasableAssets::init() {
    /** Virtual Currencies **/
    CCVirtualCurrency *gemCurrency = CCVirtualCurrency::create(
                                                                  CCString::create("Gems"),
                                                                  CCString::create("Gems currency descpription"),
                                                                  CCString::create(GEM_CURRENCY_ITEM_ID)
                                                                  );
    
    
    /** Virtual Currency Packs **/
    
    CCVirtualCurrencyPack *gems10Pack = CCVirtualCurrencyPack::create(
                                                                       CCString::create("10 Gems"), // name
                                                                       CCString::create("10 Gems Pack..."), // description
                                                                       CCString::create(GEMS_10_PACK_PRODUCT_ID), // item id
                                                                       CCInteger::create(10),	// number of currencies in the pack
                                                                       CCString::create(GEM_CURRENCY_ITEM_ID), // the currency associated with this pack
                                                                       CCPurchaseWithMarket::create(CCString::create(GEMS_10_PACK_PRODUCT_ID), CCDouble::create(0.99))
                                                                       );
    
    CCVirtualCurrencyPack *gems25Pack = CCVirtualCurrencyPack::create(
                                                                         CCString::create("50 Gems"), // name
                                                                         CCString::create("25 Gems Pack..."), // description
                                                                         CCString::create(GEMS_50_PACK_PRODUCT_ID), // item id
                                                                         CCInteger::create(50),	// number of currencies in the pack
                                                                         CCString::create(GEM_CURRENCY_ITEM_ID), // the currency associated with this pack
                                                                         CCPurchaseWithMarket::create(CCString::create(GEMS_50_PACK_PRODUCT_ID), CCDouble::create(1.99))
                                                                         );
    
    CCVirtualCurrencyPack *gems50Pack = CCVirtualCurrencyPack::create(
                                                                            CCString::create("100 Gems"), // name
                                                                            CCString::create("50 Gems Pack..."), // description
                                                                            CCString::create(GEMS_100_PACK_PRODUCT_ID), // item id
                                                                            CCInteger::create(100),	// number of currencies in the pack
                                                                            CCString::create(GEM_CURRENCY_ITEM_ID), // the currency associated with this pack
                                                                            CCPurchaseWithMarket::create(CCString::create(GEMS_100_PACK_PRODUCT_ID), CCDouble::create(4.99))
                                                                            );
    
    
    /** Virtual Goods **/
    
    /*
    
    CCVirtualGood *muffincakeGood = CCSingleUseVG::create(
                                                          CCString::create("Fruit Cake"), // name
                                                          CCString::create("Customers buy a double portion on each purchase of this cake"), // description
                                                          CCString::create("fruit_cake"), // item id
                                                          CCPurchaseWithVirtualItem::create(
                                                                                            CCString::create(GEM_CURRENCY_ITEM_ID), CCInteger::create(225)
                                                                                            ) // the way this virtual good is purchased
                                                          );
    
    CCVirtualGood *pavlovaGood = CCSingleUseVG::create(
                                                       CCString::create("Pavlova"), // name
                                                       CCString::create("Gives customers a sugar rush and they call their friends"), // description
                                                       CCString::create("pavlova"), // item id
                                                       CCPurchaseWithVirtualItem::create(CCString::create(GEM_CURRENCY_ITEM_ID), CCInteger::create(175)) // the way this virtual good is purchased
                                                       );

    
    CCVirtualGood *showRoomGood =
    CCLifetimeVG::create(CCString::create("Show Room"), // name
                         CCString::create("Show Room "), // description
                         CCString::create("show_room"), // item id
                         CCPurchaseWithVirtualItem::create(CCString::create(GEM_CURRENCY_ITEM_ID), CCInteger::create(100)));
    
    CCVirtualGood *showRoomGood0 = CCUpgradeVG::create(
                                                       CCString::create("show_room"),
                                                       NULL,
                                                       CCString::create("show_room_1"),
                                                       CCString::create("Show Room L0"),
                                                       CCString::create(""),
                                                       CCString::create("show_room_0"),
                                                       CCPurchaseWithVirtualItem::create(CCString::create(GEM_CURRENCY_ITEM_ID), CCInteger::create(220)));
    
    
    
    CCVirtualGood *deliveryVehicleGood0 = CCUpgradeVG::create(
                                                              CCString::create("delivery_vehicle"),
                                                              NULL,
                                                              CCString::create("delivery_vehicle_1"),
                                                              CCString::create("Delivery Vehicle 0"),
                                                              CCString::create(""),
                                                              CCString::create("delivery_vehicle_0"),
                                                              CCPurchaseWithVirtualItem::create(CCString::create(GEM_CURRENCY_ITEM_ID), CCInteger::create(20)));
    

    
    CCVirtualGood *fatCatGood = CCEquippableVG::create(
                                                       CCInteger::create(CCEquippableVG::kCategory),
                                                       CCString::create("Fat Cat"),
                                                       CCString::create("Fat cat description"),
                                                       CCString::create("fat_cat"),
                                                       CCPurchaseWithVirtualItem::create(CCString::create(GEM_CURRENCY_ITEM_ID), CCInteger::create(27)));
    
    */
    
    /** Virtual Categories **/
    
    /*
    
    CCVirtualCategory *cakes = CCVirtualCategory::create(
                                                         CCString::create("Cakes"),
                                                         CCArray::create(
                                                                         CCString::create("pavlova_10"),
                                                                         CCString::create("cream_cup_10"),
                                                                         NULL));
    
    CCVirtualCategory *upgrades = CCVirtualCategory::create(
                                                            CCString::create("Upgrades"),
                                                            CCArray::create(CCString::create("show_room_0"),
                                                                            CCString::create("show_room_1"),
                                                                            CCString::create("delivery_vehicle_0"),
                                                                            CCString::create("delivery_vehicle_1"),
                                                                            NULL));
    
    CCVirtualCategory *characters = CCVirtualCategory::create(
                                                              CCString::create("Characters"),
                                                              CCArray::create(CCString::create("fat_cat"), CCString::create("happi_hippo"), CCString::create("funkey_monkey"), NULL));
    
    
    */
     
    /** Google MANAGED Items **/
    
    CCNonConsumableItem *fastGemRegeneration = CCNonConsumableItem::create(
                                                                    CCString::create("Super Generator"),
                                                                    CCString::create("Generate gems faster"),
                                                                    CCString::create(FAST_REGENERATE_PRODUCT_ID),
                                                                    CCPurchaseWithMarket::createWithMarketItem(CCMarketItem::create(
                                                                                                                                    CCString::create(FAST_REGENERATE_PRODUCT_ID),
                                                                                                                                    CCInteger::create(CCMarketItem::NONCONSUMABLE), CCDouble::create(1.99))
                                                                                                               )
                                                                    );
    /*
    CCNonConsumableItem *unlockAll = CCNonConsumableItem::create(
                                                                           CCString::create("Super Unlock"),
                                                                           CCString::create("Unlock everything and get unlimited access"),
                                                                           CCString::create(FAST_REGENERATE_PRODUCT_ID),
                                                                           CCPurchaseWithMarket::createWithMarketItem(CCMarketItem::create(
                                                                                                                                           CCString::create(UNLOCK_ALL_PRODUCT_ID),
                                                                                                                                           CCInteger::create(CCMarketItem::NONCONSUMABLE), CCDouble::create(1.99))
                                                                                                                      )
                                                                           );
    */
    mCurrencies = CCArray::create(gemCurrency, NULL);
    mCurrencies->retain();
    
    //mGoods = CCArray::create(muffincakeGood, pavlovaGood, pavlovaGood, showRoomGood, showRoomGood0, deliveryVehicleGood0, fatCatGood, NULL);
    //mGoods = CCArray::create(NULL);
    //mGoods->retain();
    
    mCurrencyPacks = CCArray::create(gems10Pack, gems25Pack, gems50Pack, NULL);
    mCurrencyPacks->retain();
    
    // = CCArray::create(cakes, upgrades, characters, NULL);
    //mCategories = CCArray::create(NULL);
    //mCategories->retain();
    
    mNonConsumableItems = CCArray::create(fastGemRegeneration, //unlockAll,
                                          NULL);
    mNonConsumableItems->retain();
    
    return true;
}

StorePurchasableAssets::~StorePurchasableAssets() {
    CC_SAFE_RELEASE(mCurrencies);
    CC_SAFE_RELEASE(mGoods);
    CC_SAFE_RELEASE(mCurrencyPacks);
    CC_SAFE_RELEASE(mCategories);
    CC_SAFE_RELEASE(mNonConsumableItems);
}

int StorePurchasableAssets::getVersion() {
    return 0;
}

cocos2d::CCArray *StorePurchasableAssets::getCurrencies() {
    return mCurrencies;
}

cocos2d::CCArray *StorePurchasableAssets::getGoods() {
    return mGoods;
}

cocos2d::CCArray *StorePurchasableAssets::getCurrencyPacks() {
    return mCurrencyPacks;
}

cocos2d::CCArray *StorePurchasableAssets::getCategories() {
    return mCategories;
}

cocos2d::CCArray *StorePurchasableAssets::getNonConsumableItems() {
    return mNonConsumableItems;
}
#endif