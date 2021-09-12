// Copyright Golden Hammer Software
#include "GHHouseAd.h"
#include "GHUtils/GHMessageHandler.h"
#include "GHUtils/GHMessage.h"
#include "GHMessageTypes.h"
#include "GHGUIProperties.h"
#include "GHMath/GHRandom.h"
#include "GHGUITransitionDesc.h"

GHHouseAd::GHHouseAd(GHMessageHandler& messageQueue, GHIdentifier guiTransId)
: mMessageQueue(messageQueue)
, mCurrentlyDisplayedAd(0)
, mGUITransId(guiTransId)
{
    
}

GHHouseAd::~GHHouseAd(void)
{
    size_t numAds = mAds.size();
    for (size_t i = 0; i < numAds; ++i)
    {
        GHGUIWidgetResource* ad = mAds[i];
        ad->release();
    }
}

void GHHouseAd::activate(void)
{
    if (mCurrentlyDisplayedAd) { return; }
    
    mCurrentlyDisplayedAd = mAds[GHRandom::getRandInt((int)mAds.size())];
    
    GHMessage message(GHGUIProperties::GUIPUSH);
    message.getPayload().setProperty(GHGUIProperties::GP_WIDGETRESOURCEPTR, mCurrentlyDisplayedAd);

    GHGUITransitionDesc* trans = new GHGUITransitionDesc();
    trans->mTransitionWait = 0;
    trans->mTransitionTime = 0.4f;
    trans->mTransitionProp = mGUITransId;
    message.getPayload().setProperty(GHGUIProperties::GP_TRANSITIONDESC,
                                        GHProperty(trans, new GHRefCountedType<GHGUITransitionDesc>(trans)));

    mMessageQueue.handleMessage(message);
}

void GHHouseAd::deactivate(void)
{
    GHMessage message(GHGUIProperties::GUIPOP);
    message.getPayload().setProperty(GHGUIProperties::GP_WIDGETRESOURCEPTR, mCurrentlyDisplayedAd);
    message.getPayload().setProperty(GHGUIProperties::GP_ADDTOSTACK, false);

    GHGUITransitionDesc* trans = new GHGUITransitionDesc();
    trans->mTransitionWait = 0;
    trans->mTransitionTime = 0.4f;
    trans->mTransitionProp = mGUITransId;
    message.getPayload().setProperty(GHGUIProperties::GP_TRANSITIONDESC,
                                        GHProperty(trans, new GHRefCountedType<GHGUITransitionDesc>(trans)));

    mMessageQueue.handleMessage(message);
    mCurrentlyDisplayedAd = 0;
}

void GHHouseAd::addAd(GHGUIWidgetResource* button)
{
    button->acquire();
    mAds.push_back(button);
}
