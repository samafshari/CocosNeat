//
//  StoreEventHandler.cpp
//  GameOfDrones
//
//  Created by JAFARNEJAD Sasan on 6/1/14.
//
//

#include "StoreEventHandler.h"

#if IS_IOS
#include "Cocos2dxStore.h"
#include "helpers.h"
#include "Purchase.h"

#define TAG "StoreEventHandler >>>"

USING_NS_CC;

void StoreEventHandler::onBillingNotSupported() {
    soomla::CCStoreUtils::logDebug(TAG, "BillingNotSupported");
}

void StoreEventHandler::onBillingSupported() {
    soomla::CCStoreUtils::logDebug(TAG, "BillingSupported");
}

void StoreEventHandler::onCurrencyBalanceChanged(soomla::CCVirtualCurrency *virtualCurrency, int balance, int amountAdded) {
    soomla::CCStoreUtils::logDebug(TAG, "CurrencyBalanceChanged");
    CCNotificationCenter::sharedNotificationCenter()->postNotification
    (EVENT_ON_CURRENCY_BALANCE_CHANGED, CCInteger::create(balance));
}

void StoreEventHandler::onGoodBalanceChanged(soomla::CCVirtualGood *virtualGood, int balance, int amountAdded) {
    soomla::CCStoreUtils::logDebug(TAG, "GoodBalanceChanged");
    CCNotificationCenter::sharedNotificationCenter()->postNotification
    (EVENT_ON_GOOD_BALANCE_CHANGED, CCArray::create(virtualGood, CCInteger::create(balance), NULL));
}

void StoreEventHandler::onGoodEquipped(soomla::CCEquippableVG *equippableVG) {
    soomla::CCStoreUtils::logDebug(TAG, "GoodEquipped");
    CCNotificationCenter::sharedNotificationCenter()->postNotification(EVENT_ON_GOOD_EQUIPPED, equippableVG);
}

void StoreEventHandler::onGoodUnEquipped(soomla::CCEquippableVG *equippableVG) {
    soomla::CCStoreUtils::logDebug(TAG, "GoodUnEquipped");
    CCNotificationCenter::sharedNotificationCenter()->postNotification(EVENT_ON_GOOD_UNEQUIPPED, equippableVG);
}

void StoreEventHandler::onGoodUpgrade(soomla::CCVirtualGood *virtualGood, soomla::CCUpgradeVG *upgradeVG) {
    soomla::CCStoreUtils::logDebug(TAG, "GoodUpgrade");
    CCNotificationCenter::sharedNotificationCenter()->postNotification(EVENT_ON_GOOD_UPGRADE, virtualGood);
}

void StoreEventHandler::onItemPurchased(soomla::CCPurchasableVirtualItem *purchasableVirtualItem) {
    soomla::CCStoreUtils::logDebug(TAG, "ItemPurchased");
    log("ItemPurchased");
    Purchase::BuyPassed(purchasableVirtualItem);
}

void StoreEventHandler::onItemPurchaseStarted(soomla::CCPurchasableVirtualItem *purchasableVirtualItem) {
    soomla::CCStoreUtils::logDebug(TAG, "ItemPurchaseStarted");
}

void StoreEventHandler::onMarketPurchaseCancelled(soomla::CCPurchasableVirtualItem *purchasableVirtualItem) {
    soomla::CCStoreUtils::logDebug(TAG, "MarketPurchaseCancelled");
    log("MarketPurchaseCancelled");
    Purchase::BuyFailed(purchasableVirtualItem);
}

void StoreEventHandler::onMarketPurchase(soomla::CCPurchasableVirtualItem *purchasableVirtualItem, cocos2d::__String *token, cocos2d::__String *payload) {
    soomla::CCStoreUtils::logDebug(TAG, "MarketPurchase");
}

void StoreEventHandler::onMarketPurchaseStarted(soomla::CCPurchasableVirtualItem *purchasableVirtualItem) {
    soomla::CCStoreUtils::logDebug(TAG, "MarketPurchaseStarted");
}

void StoreEventHandler::onMarketPurchaseVerification(soomla::CCPurchasableVirtualItem *purchasableVirtualItem) {
    soomla::CCStoreUtils::logDebug(TAG, "MarketPurchaseVerification");
}

void StoreEventHandler::onRestoreTransactionsStarted() {
    soomla::CCStoreUtils::logDebug(TAG, "RestoreTransactionsStarted");
}

void StoreEventHandler::onRestoreTransactionsFinished(bool success) {
    soomla::CCStoreUtils::logDebug(TAG, "RestoreTransactionsFinished");
    Purchase::RestoreFinished(success);
}

void StoreEventHandler::onUnexpectedErrorInStore() {
    soomla::CCStoreUtils::logDebug(TAG, "UnexpectedErrorInStore");
}

void StoreEventHandler::onStoreControllerInitialized() {
    soomla::CCStoreUtils::logDebug(TAG, "StoreContorllerInitialized");
}

void StoreEventHandler::onMarketItemsRefreshed() {
    soomla::CCStoreUtils::logDebug(TAG, "MarketItemsRefreshed");
    cocos2d::__Array * currencyPacks = soomla::CCStoreInfo::sharedStoreInfo()->getVirtualCurrencyPacks();
    
    Purchase::AvailablePurchaseItems.clear();
    for (unsigned int i = 0; i < currencyPacks->count(); i++) {
        
        soomla::CCVirtualCurrencyPack *currencyPack = dynamic_cast<soomla::CCVirtualCurrencyPack*>(currencyPacks->objectAtIndex(i));
        currencyPack->retain();
        Purchase::AvailablePurchaseItems.push_back(currencyPack);
        //log(nToString((int)Purchase::AvailablePurchaseItems.size()));
        
    
        log(currencyPack->getName()->getCString());
        //Purchase::AvailablePurchaseItems[i]->retain();
        //log(currencyPack->getDescription());
        //log(currencyPack->getCurrencyAmount());
        //log(currencyPack->getCurrencyItemId());
        /*
        soomla::CCPurchaseWithMarket *purchaseWithMarket = dynamic_cast<soomla::CCPurchaseWithMarket *>(currencyPack->getPurchaseType());
        CC_ASSERT(purchaseWithMarket);
        double price = purchaseWithMarket->getMarketItem()->getPrice()->getValue();
        */
    }
    cocos2d::__Array * nonConsumableItems = soomla::CCStoreInfo::sharedStoreInfo()->getNonConsumableItems();
    for (unsigned int i = 0; i < nonConsumableItems->count(); i++) {
        
        soomla::CCNonConsumableItem *nonConsumableItem = dynamic_cast<soomla::CCNonConsumableItem*>(nonConsumableItems->objectAtIndex(i));
        soomla::CCSoomlaError *soomlaError = NULL;
        if (!soomla::CCStoreInventory::sharedStoreInventory()->nonConsumableItemExists(nonConsumableItem->getItemId()->getCString(), &soomlaError)){
            nonConsumableItem->retain();
            Purchase::AvailablePurchaseItems.push_back(nonConsumableItem);
        } else {
            //simply skip
            log("skip fast regen");
        }
        if (soomlaError) {
            soomla::CCStoreUtils::logException("Store on refresh items non consumable", soomlaError);
            return;
        }
        
        
        log(nonConsumableItem->getName()->getCString());
    }
    

}

#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)

void StoreEventHandler::onMarketRefund(soomla::CCPurchasableVirtualItem *purchasableVirtualItem) {
    soomla::CCStoreUtils::logDebug(TAG, "MarketRefund");
}

void StoreEventHandler::onIabServiceStarted() {
    soomla::CCStoreUtils::logDebug(TAG, "IabServiceStarted");
}

void StoreEventHandler::onIabServiceStopped() {
    soomla::CCStoreUtils::logDebug(TAG, "IabServiceStopped");
}
#endif
#endif