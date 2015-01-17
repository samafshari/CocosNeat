#ifndef __PURCHASE_H__
#define __PURCHASE_H__

#include "AppDelegate.h"
#include "helpers.h"
#if IS_IOS
#include "Cocos2dxStore.h"
#endif
#include "StorePurchasableAssets.h"
#include "StoreEventHandler.h"

#define GEM_CURRENCY_ITEM_ID "currency_gem"

#if NEAT_BUNDLE
#if PRO_VERSION
#define GEMS_10_PACK_PRODUCT_ID "ProPurchaseItem10Gems"
#define GEMS_50_PACK_PRODUCT_ID "ProPurchaseItemBGems"
#define GEMS_100_PACK_PRODUCT_ID "ProPurchaseItemCGems"
#define FAST_REGENERATE_PRODUCT_ID "ProPurchaseItemGemFastRegeneration"
#define UNLOCK_ALL_PRODUCT_ID "ProPurchaseItemUnlockAll"
#else
#define GEMS_10_PACK_PRODUCT_ID "PurchaseItem10Gems"
#define GEMS_50_PACK_PRODUCT_ID "PurchaseItem25Gems"
#define GEMS_100_PACK_PRODUCT_ID "PurchaseItem50Gems"
#define FAST_REGENERATE_PRODUCT_ID "PurchaseItemGemFastRegeneration"
#define UNLOCK_ALL_PRODUCT_ID "PurchaseItemUnlockAll"
#endif
#else
#if PRO_VERSION
#define GEMS_10_PACK_PRODUCT_ID "GoDProPurchaseItemAGems"
#define GEMS_50_PACK_PRODUCT_ID "GoDProPurchaseItemBGems"
#define GEMS_100_PACK_PRODUCT_ID "GoDProPurchaseItemCGems"
#define FAST_REGENERATE_PRODUCT_ID "GoDProPurchaseItemGemFastRegeneration"
#define UNLOCK_ALL_PRODUCT_ID "GoDProPurchaseItemUnlockAll"
#else
#define GEMS_10_PACK_PRODUCT_ID "GoDPurchaseItemAGems"
#define GEMS_50_PACK_PRODUCT_ID "GoDPurchaseItemBGems"
#define GEMS_100_PACK_PRODUCT_ID "GoDPurchaseItemCGems"
#define FAST_REGENERATE_PRODUCT_ID "GoDPurchaseItemGemFastRegeneration"
#define UNLOCK_ALL_PRODUCT_ID "GoDPurchaseItemUnlockAll"
#endif
#endif

#define MUFFINCAKE_ITEM_ID "fruit_cake"
#define PAVLOVA_ITEM_ID "pavlova"
#define CHOCLATECAKE_ITEM_ID "chocolate_cake"
#define CREAMCUP_ITEM_ID "cream_cup"

#if IS_IOS
typedef soomla::CCPurchasableVirtualItem PurchaseItem;
#else
typedef void PurchaseItem;
#endif

class Purchase
{
public:
    static bool RegenLocked; //if true, means we generated too much gems in one session and we should stop the timer
    static const int RegenTime = 5 * 60;
    static int RemainingTime;
    static int GemsAwardedThisSession;
    static int GetRegenTimeLeft();
    static string GetRegenTimeLeftText();
    static void FetchItems(bool quiet);
    static void ItemsFetched(bool quiet);
    static void initStore();

	static vector<PurchaseItem*> AvailablePurchaseItems;
	static void Buy(PurchaseItem*);
	static void BuyPassed(PurchaseItem*);
	static void BuyFailed(PurchaseItem*);
    static void Restore();
    static void RestoreFinished(bool success);
#if IS_IOS
	static soomla::CCEventHandler *handler;
#endif
};

#if IS_IOS
extern void ShowPopup(std::string title, std::string message);
#endif

#endif