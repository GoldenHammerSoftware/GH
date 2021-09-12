// Copyright 2010 Golden Hammer Software
#pragma once

#include "GHTransition.h"
#include "GHWPEventInterface.h"
#include "GHIdentifier.h"

class GHPropertyContainer;

class GHWPBannerAdTransition : public GHTransition
{
public:
	GHWPBannerAdTransition(PhoneDirect3DXamlAppComponent::GHWPEventInterface^ eventInterface,
		 GHPropertyContainer& props, const GHIdentifier& displayingAdsProperty);

	virtual void activate(void);
	virtual void deactivate(void);

private:
	PhoneDirect3DXamlAppComponent::GHWPEventInterface^ mEventInterface;
	GHPropertyContainer& mProps;
	GHIdentifier mDisplayingAdsProperty;
};
