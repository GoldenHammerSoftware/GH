// Copyright Golden Hammer Software
#pragma once

#include "GHUtils/GHController.h"
#include "GHGUIRenderable.h"
#include <vector>
#include "GHGUIWidget.h"
#include "GHGUIHistory.h"
#include "GHUtils/GHControllerMgr.h"
#include "GHGUIPanel.h"
#include "GHGUIChildList.h"
#include "GHGUICanvasMgr.h"

class GHRenderGroup;
class GHEventMgr;
class GHGUIMgrMessageListener;
class GHGUITransitionControllerFactory;
class GHResourceFactory;
class GHInputClaimGroup;
class GHGUITransitionDesc;
class GHStringIdFactory;
class GHGUICanvasMgr;
class GHGUIRectMaker;

// central manager for the GH gui system
// keeps track of which panels are active and sends them updates
class GHGUIMgr : public GHController
{
public:
    GHGUIMgr(GHRenderGroup& sceneRenderer,
             GHEventMgr& eventMgr,
             GHGUITransitionControllerFactory& transitionSpawner,
             GHResourceFactory& resourceFactory,
			 GHInputClaimGroup& pointerClaim,
             GHStringIdFactory& hashTable,
			 GHGUICanvasMgr& canvasMgr,
             GHGUIRectMaker& rectMaker);
    ~GHGUIMgr(void);
    
    // call shutdown to prevent any further adds or transitions.
    void shutdown(void) { mShutdown = true; }
    
    virtual void update(void);
	virtual void onActivate(void);
	virtual void onDeactivate(void);

    // make a widget active.
    //if transitionWait or Time, then run it from 0 to 1 over transitionTime after transitionWait after pushing
    void pushWidget(GHGUIWidgetResource* widget, const GHGUITransitionDesc& tDesc, float priority, size_t category);
    // push the last widget that was stored.
	void pushHistory(void);
    
    // deactivate the widget with the id.
    // if transitionWait or time, then run it from 1 to 0 over transitionTime before popping.  deactivate instantly though
    void popWidget(GHIdentifier widgetId, const GHGUITransitionDesc& tDesc, size_t category, bool storeHistory);
    void popWidget(GHGUIWidgetResource* widget, const GHGUITransitionDesc& tDesc, size_t category, bool storeHistory);
    
    GHGUIRenderable& getRenderable(void) { return mRenderable; }
    GHControllerMgr& getControllerMgr(void) { return mControllerMgr; }
	const GHGUICanvasMgr& getCanvasMgr(void) const { return mCanvasMgr; }
    GHGUICanvasMgr& getCanvasMgr(void) { return mCanvasMgr; }

    void updateScreenSize(void);

	void setCategoryActive(size_t category, bool active);
    
    // Intended purpose: to limit the gui drawing range (eg to support non-rectangular renderable surfaces)
    void setSafeWidgetArea(const GHGUIPosDesc& safePosDesc);
    
private:
    void removeWidgetInternal(GHGUIWidgetResource* widget, const GHGUITransitionDesc& tDesc, size_t category, float priority, bool storeHistory=false);

private:
    GHEventMgr& mEventMgr;
    GHRenderGroup& mSceneRenderer;
    GHGUITransitionControllerFactory& mTransitionSpawner;
	GHInputClaimGroup& mPointerClaim;
    GHControllerMgr mControllerMgr;
	GHGUIHistory mHistory;
	GHGUIRenderable mRenderable;
    GHGUIMgrMessageListener* mMessageListener;
    GHResourceFactory& mResourceFactory;
    GHStringIdFactory& mIdFactory;
    bool mShutdown;
	std::vector<GHGUIWidget::WidgetGroup> mWidgetGroups;
	GHGUICanvasMgr& mCanvasMgr;
    GHGUIRectMaker& mRectMaker;
    GHGUIWidgetResource* mTopLevelWidget{0};
};
