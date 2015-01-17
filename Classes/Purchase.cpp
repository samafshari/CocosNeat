#include "Purchase.h"
#include "BundleStoreScene.h"


int Purchase::RemainingTime = Purchase::RegenTime;
int Purchase::GemsAwardedThisSession = 0;
bool Purchase::RegenLocked = false;
vector<PurchaseItem*> Purchase::AvailablePurchaseItems;
#if IS_IOS
soomla::CCEventHandler *Purchase::handler = new StoreEventHandler();
#endif
void SystemTickSecond()
{
    GameSettings::TotalSecondsPlayed++;
    if (Purchase::GemsAwardedThisSession > 10)
    {
        Purchase::RegenLocked = true;
        Purchase::RemainingTime = Purchase::RegenTime;
        GameSettings::ReportAchievement(ACHIEVEMENT_REGEN_SESSION);
        return;
    }
    
    if (Purchase::RemainingTime == 0)
    {
        //Award Gem
        GameSettings::GiveGems(1);
        GameSettings::RegeneratedGems++;
        if (GameSettings::RegeneratedGems > 50)
            GameSettings::ReportAchievement(ACHIEVEMENT_GEM_REGEN);
        Purchase::GemsAwardedThisSession++;
        Purchase::RemainingTime = Purchase::RegenTime;
        GameSettings::SaveSettings();
    }
    
    Purchase::RemainingTime--;
    if (GameSettings::FastRechargeBought)
        Purchase::RemainingTime--;
    if (Purchase::RemainingTime < 0) Purchase::RemainingTime = 0;
    //if (GameSettings::TotalSecondsPlayed == RATE_POPUP_TIME)
    //    OpenRatePopup();
    
    //log(string_format("Timer %d", GameSettings::TotalSecondsPlayed));
}

int Purchase::GetRegenTimeLeft()
{
    return RemainingTime;
}

string Purchase::GetRegenTimeLeftText()
{
    auto timeLeft = GetRegenTimeLeft();
    int minutes = timeLeft / 60;
    int seconds = timeLeft % 60;
    string result = nToString(minutes);
    if (minutes < 10) result = "0" + result;
    result += ":";
    if (seconds < 10) result += "0";
    result += nToString(seconds);
    return result;
}

void Purchase::BuyPassed(PurchaseItem* item)
{
#if IS_IOS
    string itemName = item->getItemId()->getCString();
    log(itemName);
    
    if (itemName == FAST_REGENERATE_PRODUCT_ID)
    {
        GameSettings::ReportAchievement(ACHIEVEMENT_BUY_REGEN);
        GameSettings::FastRechargeBought = true;
        GameSettings::SaveSettings();
    }
    else
    {
        GameSettings::ReportAchievement(ACHIEVEMENT_BUY_GEM);
        if (itemName == GEMS_10_PACK_PRODUCT_ID)
            GameSettings::GiveGems(10);
        else if (itemName == GEMS_50_PACK_PRODUCT_ID)
            GameSettings::GiveGems(50);
        else if (itemName == GEMS_100_PACK_PRODUCT_ID)
            GameSettings::GiveGems(100);
    }
#endif

    //callback from Buy, show sth
    if (EngineRunning)
    Director::getInstance()->replaceScene(
        TransitionFade::create(0.5f, BundleStore::createBuyPassedScene()));
}

void Purchase::BuyFailed(PurchaseItem* item)
{
    //callback from Buy, show sth
    if (EngineRunning)
    Director::getInstance()->replaceScene(
        TransitionFade::create(0.5f, BundleStore::createBuyFailedScene()));
}

void Purchase::ItemsFetched(bool quiet)
{
    //callback from FetchItems,
    //do nothing if quiet; show store if not quiet
    if (!quiet && EngineRunning)
        Director::getInstance()->replaceScene(
            TransitionFade::create(0.5f, BundleStore::createStoreScene()));
}

void Purchase::initStore(){
#if IS_IOS
    // We initialize CCStoreController and the event handler before
    // we open the store.
    
    
    soomla::CCSoomla::sharedSoomla()->addEventHandler(Purchase::handler);
    
    StorePurchasableAssets *assets = StorePurchasableAssets::create();
    __Dictionary *storeParams = __Dictionary::create();
    storeParams->setObject(__String::create("123"), "androidPublicKey");
    storeParams->setObject(__String::create("123"), "customSecret");
    
    __String *soomSec = __String::create("123123123");
    soomla::CCStoreController::sharedStoreController()->setSoomSec(soomSec);
    
    // This is the call to initialize CCStoreController
    soomla::CCStoreController::initShared(assets, storeParams);
    
    /*
     * ** Set the amount of each currency to 10,000 if the **
     * ** balance drops under 1,000 **
     *
     * ** Of course, this is just for testing... **
     */
    
    CCArray *currencies = soomla::CCStoreInfo::sharedStoreInfo()->getVirtualCurrencies();
    CCObject *currencyObject;
    CCARRAY_FOREACH(currencies, currencyObject) {
        soomla::CCVirtualCurrency *vc =
        dynamic_cast<soomla::CCVirtualCurrency *>(currencyObject);
        int balance = soomla::CCStoreInventory::sharedStoreInventory()->
        getItemBalance(vc->getItemId()->getCString(), NULL);
        if (balance < 15) {
            soomla::CCStoreInventory::sharedStoreInventory()->
            giveItem(vc->getItemId()->getCString(), 100 - balance, NULL);
        }
    }
    ///////////////////////////////////////////////////////////////////////
#endif
}

void Purchase::FetchItems(bool quiet)
{
	//get items from app store,
	//put them into Purchase::AvailablePurchaseItems
	//call Purchase::ItemsFetched(quiet)
}

void Purchase::Buy(PurchaseItem* item)
{
#if IS_IOS
	soomla::CCSoomlaError *soomlaError = NULL;
	soomla::CCStoreInventory::sharedStoreInventory()->buyItem(item->getItemId()->getCString(), &soomlaError);
	if (soomlaError) {
		soomla::CCStoreUtils::logException("StoreBScene::onBuy", soomlaError);
		return;
	}
#endif
}

void Purchase::Restore()
{
#if IS_IOS
	soomla::CCStoreController::sharedStoreController()->restoreTransactions();
#endif
}


void Purchase::RestoreFinished(bool success)
{
    if (EngineRunning)
        Director::getInstance()->replaceScene(
            TransitionFade::create(0.5f, BundleStore::createRestorePassedScene(success)));
}