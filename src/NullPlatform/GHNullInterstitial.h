#pragma once

#include "GHAdTransition.h"
#include "GHString/GHIdentifier.h"

class GHMessageHandler;

class GHNullInterstitial : public GHAdTransition
{
public:
	GHNullInterstitial(GHMessageHandler& messageHandler, const GHIdentifier& finishedMessageId);

	virtual bool hasAd(void) const;
	virtual void activate(void);
	virtual void deactivate(void);

private:
	GHIdentifier mFinishedMessageId;
	GHMessageHandler& mMessageHandler;
};

