// Copyright 2010 Golden Hammer Software
#include "GHMetroAdBannerController.h"
#include "GHUtils/GHPropertyContainer.h"
#include "GHPlatform/GHDebugMessage.h"
#include "GHUtils/GHMessage.h"
#include "GHUtils/GHEventMgr.h"
#include "GHMessageTypes.h"
#include "GHScreenInfo.h"

namespace GHMETROAD_NAMESPACE 
{
	AdCallbackHandler::AdCallbackHandler(GHMetroAdBannerController& parent)
		: mParent(parent)
	{
	}
	void AdCallbackHandler::adError(Platform::Object^ object, Microsoft::Advertising::WinRT::UI::AdErrorEventArgs^ e)
	{
		// we want to assume the banner always has an ad even if errors happen.
		// the ad won't continue looking if it is hidden.

		//MicrosoftAdvertising::ErrorCode errCode = e->ErrorCode;
		//mParent.adError();
	}
	void AdCallbackHandler::adRefreshed(Platform::Object^ object, Windows::UI::Xaml::RoutedEventArgs^ e)
	{
		mParent.adRefreshed();
	}
	void AdCallbackHandler::isEngagedChanged(Platform::Object^ object, Windows::UI::Xaml::RoutedEventArgs^ e)
	{
		mParent.isEngagedChanged();
	}
}

GHMetroAdBannerController::GHMetroAdBannerController(Microsoft::Advertising::WinRT::UI::AdControl^ adXamlControl,
													GHPropertyContainer& props, const GHIdentifier& displayingAdsProperty,
													GHEventMgr& eventMgr, const GHScreenInfo& screenInfo,
													Windows::UI::Core::CoreDispatcher^ uiDispatcher)
: mAdXaml(adXamlControl)
, mProps(props)
, mDisplayingAdsProperty(displayingAdsProperty)
, mCallbackHandler(nullptr)
, mHasAd(false)
, mAdStatusChanged(false)
, mEventMgr(eventMgr)
, mScreenInfo(screenInfo)
, mMessageHandler(*this)
, mAdIsVisible(false)
, mUIDispatcher(uiDispatcher)
{
	if (mAdXaml)
	{
		mUIDispatcher->RunAsync(
			Windows::UI::Core::CoreDispatcherPriority::Normal,
			ref new Windows::UI::Core::DispatchedHandler([this]()
		{
			mAdXaml->ApplicationId = GHMETROAD_APPID;
			mAdXaml->AdUnitId = GHMETROAD_ADUNITID;
			// make sure ads are disabled until this controller is active.
			//mAdXaml->IsEnabled = false;
			//mAdXaml->Visibility =  Windows::UI::Xaml::Visibility::Collapsed;
			//mAdXaml->Suspend(false);

			mCallbackHandler = ref new GHMETROAD_NAMESPACE::AdCallbackHandler(*this);
			mAdXaml->AdRefreshed += ref new Windows::Foundation::EventHandler<Windows::UI::Xaml::RoutedEventArgs^>(mCallbackHandler, &GHMETROAD_NAMESPACE::AdCallbackHandler::adRefreshed);
			mAdXaml->IsEngagedChanged += ref new Windows::Foundation::EventHandler<Windows::UI::Xaml::RoutedEventArgs^>(mCallbackHandler, &GHMETROAD_NAMESPACE::AdCallbackHandler::isEngagedChanged);
			mAdXaml->ErrorOccurred += ref new Windows::Foundation::EventHandler<Microsoft::Advertising::WinRT::UI::AdErrorEventArgs^>(mCallbackHandler, &GHMETROAD_NAMESPACE::AdCallbackHandler::adError);
		}));
	}
	mProps.setProperty(mDisplayingAdsProperty, 0);
}

void GHMetroAdBannerController::update(void)
{
	if (mAdStatusChanged)
	{
		mAdStatusChanged = false;
		if (mHasAd) 
		{
			if (mScreenInfo.getSize()[0] > 700) {
				showAd();
			}
			else {
				hideAd();
			}
		}
		else {
			mProps.setProperty(mDisplayingAdsProperty, 0);
			// we don't want to hide the ad if we have not recieved one from the server yet.
		}
	}
}

void GHMetroAdBannerController::onActivate(void)
{
	if (!mAdXaml) return;
	mEventMgr.registerListener(GHMessageTypes::WINDOWRESIZE, mMessageHandler);
	showAd();
	mAdStatusChanged = true;
}

void GHMetroAdBannerController::onDeactivate(void)
{
	if (!mAdXaml) return;
	mEventMgr.unregisterListener(GHMessageTypes::WINDOWRESIZE, mMessageHandler);
	hideAd();
}

void GHMetroAdBannerController::showAd(void)
{
	if (!mAdIsVisible)
	{
		mUIDispatcher->RunAsync(
			Windows::UI::Core::CoreDispatcherPriority::Normal,
			ref new Windows::UI::Core::DispatchedHandler([this]()
		{
			//mAdXaml->Resume();
			//mAdXaml->IsEnabled = true;
			mAdXaml->Visibility = Windows::UI::Xaml::Visibility::Visible;
			mAdIsVisible = true;
		}));
	}
	if (mHasAd) {
		mProps.setProperty(mDisplayingAdsProperty, 1);
	}
}

void GHMetroAdBannerController::hideAd(void)
{
	if (mAdIsVisible)
	{
		mUIDispatcher->RunAsync(
			Windows::UI::Core::CoreDispatcherPriority::Normal,
			ref new Windows::UI::Core::DispatchedHandler([this]()
		{
			//mAdXaml->Suspend(false);
			//mAdXaml->IsEnabled = false;
			mAdXaml->Visibility = Windows::UI::Xaml::Visibility::Collapsed;
			mAdIsVisible = false;
		}));
	}
	mProps.setProperty(mDisplayingAdsProperty, 0);
}

void GHMetroAdBannerController::adError(void)
{
	GHDebugMessage::outputString("Ad Error");
	mHasAd = false;
	mAdStatusChanged = true;
}

void GHMetroAdBannerController::adRefreshed(void)
{
	//GHDebugMessage::outputString("Ad Refreshed");
	mHasAd = true;
	mAdStatusChanged = true;
}

void GHMetroAdBannerController::isEngagedChanged(void)
{
	//GHDebugMessage::outputString("Ad Engaged Changed");
	if (mAdXaml->IsEngaged)
	{
		mEventMgr.handleMessage(GHMessage(GHMessageTypes::PAUSEINTERRUPT));
	}
	else
	{
		mEventMgr.handleMessage(GHMessage(GHMessageTypes::UNPAUSEINTERRUPT));
	}
}

void GHMetroAdBannerController::handleWindowSizeChanged(void)
{
	mAdStatusChanged = true;
}

void GHMetroAdBannerController::MessageHandler::handleMessage(const GHMessage& message)
{
	mParent.handleWindowSizeChanged();
}

