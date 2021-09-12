#include "GHNullInterstitial.h"
#include "GHUtils/GHMessageHandler.h"
#include "GHUtils/GHMessage.h"

GHNullInterstitial::GHNullInterstitial(GHMessageHandler& messageHandler, const GHIdentifier& finishedMessageId)
	: mFinishedMessageId(finishedMessageId)
	, mMessageHandler(messageHandler)
{
}

bool GHNullInterstitial::hasAd(void) const
{
	return true;
}

void GHNullInterstitial::activate(void)
{
	mMessageHandler.handleMessage(GHMessage(mFinishedMessageId));
}

void GHNullInterstitial::deactivate(void)
{
	
}
