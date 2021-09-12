// Copyright Golden Hammer Software
#include "GHGUIHistory.h"
#include "GHGUIMgr.h"
#include "GHPlatform/GHDebugMessage.h"

GHGUIHistory::GHGUIHistory(GHGUIMgr& mgr)
: mGUIMgr(mgr)
, mMaxHistory(4) // arbitrary
{
}

GHGUIHistory::~GHGUIHistory(void)
{
    for (size_t i = 0; i < mHistory.size(); ++i) {
        delete mHistory[i];
    }
}

void GHGUIHistory::storePanel(GHGUIWidgetResource* res, const GHGUITransitionDesc& tDesc, size_t category, float priority)
{
    if (!res) return;
    if (mHistory.size()+1 >= mMaxHistory) {
        Entry* entry = *mHistory.begin();
        mHistory.erase(mHistory.begin());
        delete entry;
    }
    Entry* newEntry = new Entry(res, tDesc, category, priority);
    mHistory.push_back(newEntry);
}

void GHGUIHistory::goBack(void)
{
    if (!mHistory.size()) {
        GHDebugMessage::outputString("Failed to find a history gui to go back to.");
        return;
    }
    mHistory[mHistory.size()-1]->apply(mGUIMgr);
    Entry* entry = *mHistory.rbegin();
    mHistory.pop_back();
    delete entry;
}

GHGUIHistory::Entry::Entry(GHGUIWidgetResource* res, const GHGUITransitionDesc& tDesc, size_t category, float priority)
: mRes(0)
, mDesc(tDesc)
, mCategory(category)
, mPriority(priority)
{
    assert(res != 0);
    GHRefCounted::changePointer((GHRefCounted*&)mRes, res);
}

GHGUIHistory::Entry::~Entry(void)
{
    GHRefCounted::changePointer((GHRefCounted*&)mRes, 0);
}

void GHGUIHistory::Entry::apply(GHGUIMgr& mgr)
{
    mgr.pushWidget(mRes, mDesc, mPriority, mCategory);
}
