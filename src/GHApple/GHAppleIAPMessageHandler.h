// Copyright 2010 Golden Hammer Software
#pragma once

#include "GHUtils/GHMessageHandler.h"
#include <list>
#include <vector>

class GHIAPCallback;
class GHAppleIAPStore;

class GHAppleIAPMessageHandler : public GHMessageHandler
{
public:
    GHAppleIAPMessageHandler(GHAppleIAPStore& store);
    virtual void handleMessage(const GHMessage& message);
    
    void registerCallback(GHIAPCallback* callback);
    void unregisterCallback(GHIAPCallback* callback);
    void getCallbacks(std::vector<GHIAPCallback*>& ret) const;
    bool hasCallbacks(void) const;
    
private:
    GHAppleIAPStore& mStore;
    //All callbacks will be informed of all transactions
    typedef std::list<GHIAPCallback*> CallbackList;
    CallbackList mCallbacks;
};
