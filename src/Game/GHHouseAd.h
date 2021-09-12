// Copyright Golden Hammer Software
#pragma once

#include <vector>
#include "GUI/GHGUIWidget.h"
#include "Game/GHAdTransition.h"
#include "GHString/GHIdentifier.h"

class GHMessageHandler;

// A transition that randomly picks from a set of widgets to apply
class GHHouseAd : public GHAdTransition
{
public:
    GHHouseAd(GHMessageHandler& messageQueue, GHIdentifier guiTransId);
    ~GHHouseAd(void);
    
    // show ad
	virtual void activate(void);
    // hide ad
	virtual void deactivate(void);
    
    void addAd(GHGUIWidgetResource* button);
   
    virtual bool hasAd(void) const { return (mAds.size() > 0); }

private:
    GHMessageHandler& mMessageQueue;
    GHIdentifier mGUITransId;
    
    std::vector<GHGUIWidgetResource*> mAds;
    GHGUIWidgetResource* mCurrentlyDisplayedAd;
};
