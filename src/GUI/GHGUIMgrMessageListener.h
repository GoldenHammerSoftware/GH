// Copyright Golden Hammer Software
#pragma once

#include "GHUtils/GHMessageHandler.h"
#include "GHGUIWidget.h"

class GHGUIMgr;
class GHEventMgr;
class GHResourceFactory;
class GHStringIdFactory;

class GHGUIMgrMessageListener : public GHMessageHandler
{
public:
    GHGUIMgrMessageListener(GHGUIMgr& guiMgr, GHEventMgr& eventMgr,
                            GHResourceFactory& resourceFactory,
                            GHStringIdFactory& hashTable);
    ~GHGUIMgrMessageListener(void);
    
    virtual void handleMessage(const GHMessage& message);
    
private:
    GHGUIWidgetResource* getWidgetResource(const GHMessage& message);
    void handleGUIPopMessage(const GHMessage& message);

private:
    GHGUIMgr& mGUIMgr;
    GHEventMgr& mEventMgr;
    GHResourceFactory& mResourceFactory;
    GHStringIdFactory& mIdFactory;
};
