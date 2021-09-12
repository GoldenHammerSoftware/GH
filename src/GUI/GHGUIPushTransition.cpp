// Copyright Golden Hammer Software
#include "GHGUIPushTransition.h"
#include "GHGUIMgr.h"
#include "GHUtils/GHResourceFactory.h"

GHGUIPushTransition::GHGUIPushTransition(GHGUIMgr& guiMgr, GHGUIWidgetResource* widget,
                                         const GHGUITransitionDesc& tDesc, float priority, size_t category)
: mGUIMgr(guiMgr)
, mWidget(0)
, mKeepLoaded(false)
, mTDesc(tDesc)
, mPriority(priority)
, mCategory(category)
{
    GHRefCounted::changePointer((GHRefCounted*&)mWidget, widget);
}

GHGUIPushTransition::GHGUIPushTransition(GHGUIMgr& guiMgr, GHResourceFactory& resourceFactory, const char* resourceName,
                                         const GHGUITransitionDesc& tDesc, float priority, size_t category, bool keepLoaded)
: mGUIMgr(guiMgr)
, mResourceFactory(&resourceFactory)
, mKeepLoaded(keepLoaded)
, mResourceName(resourceName, GHString::CHT_COPY)
, mWidget(0)
, mTDesc(tDesc)
, mPriority(priority)
, mCategory(category)
{
}

GHGUIPushTransition::~GHGUIPushTransition(void)
{
    GHRefCounted::changePointer((GHRefCounted*&)mWidget, 0);
}

void GHGUIPushTransition::activate(void)
{
    if (!mWidget)
    {
        GHGUIWidgetResource* widget = (GHGUIWidgetResource*)mResourceFactory->getCacheResource(mResourceName.getChars());
        if (!widget) return;
        mGUIMgr.pushWidget(widget, mTDesc, mPriority, mCategory);

        if (mKeepLoaded)
        {
            mWidget = widget;
            mWidget->acquire();
        }
    }
    else
    {
        mGUIMgr.pushWidget(mWidget, mTDesc, mPriority, mCategory);
    }
}
