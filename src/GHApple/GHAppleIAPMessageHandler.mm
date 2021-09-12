// Copyright 2010 Golden Hammer Software
#include "GHAppleIAPMessageHandler.h"
#include "GHAppleIdentifiers.h"
#include "GHUtils/GHMessage.h"
#include "GHUtils/GHPropertyContainer.h"
#include "GHIAPCallback.h"
#include <algorithm>
#include "GHAppleIAPStore.h"

GHAppleIAPMessageHandler::GHAppleIAPMessageHandler(GHAppleIAPStore& store)
: mStore(store)
{
    
}

void GHAppleIAPMessageHandler::handleMessage(const GHMessage& message)
{
    if (message.getType() == GHAppleIdentifiers::IAPINFORMGAME)
    {
        bool success = message.getPayload().getProperty(GHAppleIdentifiers::IAPSUCCESS);
        int itemId = message.getPayload().getProperty(GHAppleIdentifiers::IAPITEMID);
        CallbackList::iterator iter = mCallbacks.begin(), iterEnd = mCallbacks.end();
        for (; iter != iterEnd; ++iter)
        {
            GHIAPCallback* callback = *iter;
            callback->transactionComplete(success, itemId);
        }
    }
    else if (message.getType() == GHAppleIdentifiers::IAPRESTORE)
    {
        mStore.restorePurchases();
    }
}

bool GHAppleIAPMessageHandler::hasCallbacks(void) const
{
    return mCallbacks.size();
}

void GHAppleIAPMessageHandler::registerCallback(GHIAPCallback* callback)
{
    mCallbacks.push_back(callback);
}

void GHAppleIAPMessageHandler::getCallbacks(std::vector<GHIAPCallback*>& ret) const
{
    ret.clear();
    for (CallbackList::const_iterator iter = mCallbacks.begin(); iter != mCallbacks.end(); iter++)
    {
        ret.push_back(*iter);
    }
}

void GHAppleIAPMessageHandler::unregisterCallback(GHIAPCallback* callback)
{
    CallbackList::iterator iter = std::find(mCallbacks.begin(), mCallbacks.end(), callback);
    if (iter != mCallbacks.end())
    {
        mCallbacks.erase(iter);
    }
}
