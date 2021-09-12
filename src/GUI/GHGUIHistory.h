// Copyright Golden Hammer Software
#pragma once

#include "GHGUIWidget.h"
#include "GHGUITransitionDesc.h"

class GHGUIMgr;

// util class for remembering what panels used to be up, and returning to them.
class GHGUIHistory
{
public:
    GHGUIHistory(GHGUIMgr& mgr);
    ~GHGUIHistory(void);

    // remember a panel along with the values used to push it.
    void storePanel(GHGUIWidgetResource* res, const GHGUITransitionDesc& tDesc, size_t category, float priority);
    // push the most recent addition back onto the gui mgr.
    void goBack(void);
    
private:
    GHGUIMgr& mGUIMgr;
    
    // struct to remember one history item.
    class Entry {
    public:
        Entry(GHGUIWidgetResource* res, const GHGUITransitionDesc& tDesc, size_t category, float priority);
        ~Entry(void);

        // put our resource back onto the gui mgr.
        void apply(GHGUIMgr& mgr);
        
    private:
        GHGUIWidgetResource* mRes;
		const GHGUITransitionDesc mDesc;
		size_t mCategory;
		float mPriority;
	};
    std::vector<Entry*> mHistory;
    // a val to say how long our history can be
    // prevents flowing out into infinity with uncareful use.
    unsigned int mMaxHistory;
};
