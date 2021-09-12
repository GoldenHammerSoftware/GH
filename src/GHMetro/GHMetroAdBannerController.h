// Copyright 2010 Golden Hammer Software
#pragma once

// We expect each game that uses this to provide a GHMetroAdDefinitions.h file,
//  which contains the appropriate namespace definition GHMETROAD_NAMESPACE,
//  an application id string GHMETROAD_APPID
//  and a ad unit id string GHMETROAD_ADUNITID
#include "GHMetroAdDefinitions.h"

#include "GHUtils/GHController.h"
#include "GHString/GHIdentifier.h"
#include "GHUtils/GHMessageHandler.h"

class GHPropertyContainer;
class GHMetroAdBannerController;
class GHEventMgr;
class GHScreenInfo;

namespace GHMETROAD_NAMESPACE 
{
	public ref class AdCallbackHandler sealed
	{
		friend class GHMetroAdBannerController;

	private:
		AdCallbackHandler(GHMetroAdBannerController& parent);

	public:
		// callbacks from xaml
		void adError(Platform::Object^ object, Microsoft::Advertising::WinRT::UI::AdErrorEventArgs^ e);
		void adRefreshed(Platform::Object^ object, Windows::UI::Xaml::RoutedEventArgs^ e);
		void isEngagedChanged(Platform::Object^ object, Windows::UI::Xaml::RoutedEventArgs^ e);

	private:
		GHMetroAdBannerController& mParent;
	};
};

class GHMetroAdBannerController : public GHController
{
public:
	GHMetroAdBannerController(Microsoft::Advertising::WinRT::UI::AdControl^ adXamlControl,
		GHPropertyContainer& props, const GHIdentifier& displayingAdsProperty, 
		GHEventMgr& eventMgr, const GHScreenInfo& screenInfo,
		Windows::UI::Core::CoreDispatcher^ uiDispatcher);

	virtual void update(void);
	virtual void onActivate(void);
	virtual void onDeactivate(void);

	void adError(void);
	void adRefreshed(void);
	void isEngagedChanged(void);

	// callback from the message handler.
	void handleWindowSizeChanged(void);

private:
	void showAd(void);
	void hideAd(void);

private:
	class MessageHandler : public GHMessageHandler
	{
	public:
		MessageHandler(GHMetroAdBannerController& parent) : mParent(parent) {}
		virtual void handleMessage(const GHMessage& message);

	private:
		GHMetroAdBannerController& mParent;
	};

private:
	GHMETROAD_NAMESPACE::AdCallbackHandler^ mCallbackHandler;
	Microsoft::Advertising::WinRT::UI::AdControl^ mAdXaml;
	Windows::UI::Core::CoreDispatcher^ mUIDispatcher;

	GHPropertyContainer& mProps;
	GHIdentifier mDisplayingAdsProperty;
	// flag to say whether we think we have an ad or not
	bool mHasAd;
	// flag to say something about the ad changed so we can handle it during update.
	bool mAdStatusChanged;
	// for sending interrupts.
	// also for registering for window size messages.
	GHEventMgr& mEventMgr;
	// for checking window size.
	const GHScreenInfo& mScreenInfo;
	MessageHandler mMessageHandler;
	// a flag to say whether or not we have active xaml.
	bool mAdIsVisible;
};
