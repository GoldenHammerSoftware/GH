// Copyright 2010 Golden Hammer Software
#include "GHWPBannerAdTransition.h"
#include "GHPropertyContainer.h"

GHWPBannerAdTransition::GHWPBannerAdTransition(PhoneDirect3DXamlAppComponent::GHWPEventInterface^ eventInterface,
											   GHPropertyContainer& props, const GHIdentifier& displayingAdsProperty)
: mEventInterface(eventInterface)
, mProps(props)
, mDisplayingAdsProperty(displayingAdsProperty)
{
}

void GHWPBannerAdTransition::activate(void)
{
	mEventInterface->showAds();
	mProps.setProperty(mDisplayingAdsProperty, true);
}

void GHWPBannerAdTransition::deactivate(void)
{
	mEventInterface->hideAds();
	mProps.setProperty(mDisplayingAdsProperty, false);
}
