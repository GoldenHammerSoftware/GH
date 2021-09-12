// Copyright 2010 Golden Hammer Software
#include "GHMetroAdFactory.h"
#include "GHMetroAdBannerController.h"
#include "GHUtils/GHRandomTransition.h"
#include "GHInstantAbortAd.h"
#include "GHUtils/GHEventMgr.h"
#include "GHUtils/GHControllerTransition.h"
#include "GHInterstitialMgrTransition.h"
#include <cassert>

GHMetroAdFactory::GHMetroAdFactory(Microsoft::Advertising::WinRT::UI::AdControl^ adXamlControl,
								   GHEventMgr& eventMgr, const GHScreenInfo& screenInfo,
								   GHControllerMgr& uiControllerMgr)
: mAdXamlControl(adXamlControl)
, mEventMgr(eventMgr)
, mInterstitialDelegate(eventMgr)
, mScreenInfo(screenInfo)
, mUIControllerMgr(uiControllerMgr)
, mUIDispatcher(Windows::ApplicationModel::Core::CoreApplication::MainView->CoreWindow->Dispatcher)
{
}

GHTransition* GHMetroAdFactory::createBannerAds(const GHTimeVal& timer, GHPropertyContainer& props, 
												const GHIdentifier& displayingAdsProperty,
												GHHouseAd* houseAd)
{
	if (mAdXamlControl == nullptr) return 0;
	GHMetroAdBannerController* adController = new GHMetroAdBannerController(mAdXamlControl, props, displayingAdsProperty, mEventMgr, mScreenInfo, mUIDispatcher);
	GHControllerTransition* adTrans = new GHControllerTransition(mUIControllerMgr, adController);
	return adTrans;
}
    
GHInterstitialMgrTransition* GHMetroAdFactory::createInterstitialAds(void)
{
	GHInterstitialMgrTransition* ret = new GHInterstitialMgrTransition();
    
    GHInstantAbortAd* fallbackAd = new GHInstantAbortAd(mInterstitialDelegate);
    ret->addChild(fallbackAd, 1);

	return ret;
}

GHInterstitialMgrTransition* GHMetroAdFactory::createRewardInterstitialAds(GHPropertyContainer& props, const GHIdentifier& availabilityProp)
{
	GHInterstitialMgrTransition* ret = new GHInterstitialMgrTransition();

	GHInstantAbortAd* fallbackAd = new GHInstantAbortAd(mInterstitialDelegate);
	ret->addChild(fallbackAd, 1);

	return ret;
}


