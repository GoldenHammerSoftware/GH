// Copyright Golden Hammer Software
#include "GHGUIMgr.h"
#include "GHRenderGroup.h"
#include "GHGUIMgrMessageListener.h"
#include "GHGUITransitionControllerFactory.h"
#include "GHPlatform/GHDebugMessage.h"
#include "GHInputClaimGroup.h"
#include "GHGUITransitionDesc.h"
#include "GHGUIPanel.h"
#include "GHGUIRectMaker.h"

GHGUIMgr::GHGUIMgr(GHRenderGroup& sceneRenderer,
				   GHEventMgr& eventMgr,
				   GHGUITransitionControllerFactory& transitionSpawner,
				   GHResourceFactory& resourceFactory,
				   GHInputClaimGroup& pointerClaim,
				   GHStringIdFactory& hashTable,
				   GHGUICanvasMgr& canvasMgr,
                   GHGUIRectMaker& rectMaker)
	: mSceneRenderer(sceneRenderer)
	, mEventMgr(eventMgr)
	, mMessageListener(0)
	, mTransitionSpawner(transitionSpawner)
	, mResourceFactory(resourceFactory)
	, mShutdown(false)
	, mHistory(*this)
	, mPointerClaim(pointerClaim)
	, mIdFactory(hashTable)
	, mCanvasMgr(canvasMgr)
    , mRectMaker(rectMaker)
{
	mWidgetGroups.resize(1);
	mWidgetGroups[0].mIsActive = true;
}

GHGUIMgr::~GHGUIMgr(void)
{
	for (auto groupIter = mWidgetGroups.begin(); groupIter != mWidgetGroups.end(); ++groupIter)
	{
		for (size_t i = 0; i < (*groupIter).mWidgets.size(); ++i)
		{
			(*groupIter).mWidgets[i]->release();
		}
	}
    if (mTopLevelWidget)
    {
        mTopLevelWidget->release();
    }
}

void GHGUIMgr::update(void)
{
	mPointerClaim.clearClaims();
	mCanvasMgr.update(mWidgetGroups);
    mControllerMgr.update();

    // todo: do a lazy depth evaluation only when things change.
	int depth = 0;
	// djw: why were we assigning separate depths to transitions?
	//  shouldn't they be in the widget groups?
	//  not iterating them doesn't seem to break anything...
	//  ...and if we do this first then the sorting gets strange for overlapping guis.
	//  It should be the same as a no-op to calc depth on transition spawner and then calc depth on the groups,
	//   since the depth will be overwritten by the group depth.  Something is up.
	//int depth = mTransitionSpawner.calcDepth(0);
	for (auto groupIter = mWidgetGroups.begin(); groupIter != mWidgetGroups.end(); ++groupIter)
	{
		if ((*groupIter).mIsActive)
		{
			for (size_t i = 0; i < (*groupIter).mWidgets.size(); ++i)
			{
				depth = (*groupIter).mWidgets[i]->get()->calcDepth(depth);
			}
		}
	}
}

void GHGUIMgr::onActivate(void)
{
    mSceneRenderer.addRenderable(mRenderable, 0);
    mMessageListener = new GHGUIMgrMessageListener(*this, mEventMgr, mResourceFactory, mIdFactory);
}

void GHGUIMgr::onDeactivate(void)
{
    mSceneRenderer.removeRenderable(mRenderable, 0);
    delete mMessageListener;
}

void GHGUIMgr::setSafeWidgetArea(const GHGUIPosDesc& safePosDesc)
{
    if (!mTopLevelWidget)
    {
        mTopLevelWidget = new GHGUIWidgetResource(new GHGUIPanel(mRenderable, mRectMaker));
        mTopLevelWidget->acquire();
        
        size_t numWidgetGroups = mWidgetGroups.size();
        for (size_t g = 0; g < numWidgetGroups; ++g)
        {
            // This code is largely untested since the only case it was tested with
            // was in an app with only a single always-active WidgetGroup. This comment can be removed if obsolete.
            GHGUIWidget::WidgetGroup& widgetGroup = mWidgetGroups[g];
            size_t numWidgets = widgetGroup.mWidgets.size();
            for (size_t w = 0; w < numWidgets; ++w)
            {
                mTopLevelWidget->get()->addChild(widgetGroup.mWidgets[w]);
            }
        }
    }
    
    mTopLevelWidget->get()->setPosDesc(safePosDesc);
    mTopLevelWidget->get()->updatePosition();

	const GHRect<float, 2>& safeRect = mTopLevelWidget->get()->getScreenPos();
	GHPoint2 safeSize;
	safeRect.calculateDimensions(safeSize);
	mRectMaker.setSafeAreaPct(safeSize);
}

void GHGUIMgr::pushWidget(GHGUIWidgetResource* widget, const GHGUITransitionDesc& tDesc, float priority, size_t category)
{
    if (mShutdown) return;
    
    if (mWidgetGroups.size() <= category)
	{
		mWidgetGroups.resize(category + 1);
	}

	if (mWidgetGroups[category].mWidgets.contains(widget)) {
		// don't add twice.
		GHDebugMessage::outputString("Tried to add gui widget %s twice!", widget->getResourceId().getString());
		return;
	}
    
    if (mTopLevelWidget) {
        mTopLevelWidget->get()->addChild(widget);
    }
    
	mWidgetGroups[category].mWidgets.add(widget, priority);
    widget->acquire();
    
	if (mWidgetGroups[category].mIsActive)
	{
		if (tDesc.mTransitionTime || tDesc.mTransitionWait || !tDesc.mTransitionProp.isNull()) {
			mTransitionSpawner.spawnTransition(widget, mTopLevelWidget, tDesc, true);
		}
		else {
			mTransitionSpawner.abortTransitions(widget);
			widget->get()->enterTransitionIn();
			widget->get()->finishTransitionIn();
		}
	}
}

void GHGUIMgr::pushHistory(void)
{ 
	mHistory.goBack();
}

void GHGUIMgr::popWidget(GHIdentifier widgetId, const GHGUITransitionDesc& tDesc, size_t category, bool storeHistory)
{
	assert(category < mWidgetGroups.size());

	GHGUIWidgetResource* found = mWidgetGroups[category].mWidgets.find(widgetId);
	if (!found) return;
	float priority = mWidgetGroups[category].mWidgets.findPriority(widgetId);

	removeWidgetInternal(found, tDesc, category, priority, storeHistory);
    found->release();
	mWidgetGroups[category].mWidgets.remove(found);
}

void GHGUIMgr::popWidget(GHGUIWidgetResource* widget, const GHGUITransitionDesc& tDesc, size_t category, bool storeHistory)
{
	if (!widget) return;

	float priority = mWidgetGroups[category].mWidgets.findPriority(widget->get()->getId());
	if (mWidgetGroups[category].mWidgets.remove(widget))
	{
        removeWidgetInternal(widget, tDesc, category, priority, storeHistory);
        widget->release();
	}
}

void GHGUIMgr::removeWidgetInternal(GHGUIWidgetResource* widget, const GHGUITransitionDesc& tDesc, size_t category, float priority, bool storeHistory)
{
	assert(category < mWidgetGroups.size());
	
	bool didSpawnTransition = false;

	if (mWidgetGroups[category].mIsActive)
	{
		if (!mShutdown && (tDesc.mTransitionTime || tDesc.mTransitionWait)) {
			mTransitionSpawner.spawnTransition(widget, mTopLevelWidget, tDesc, false);
			didSpawnTransition = true;
		}
		else {
			mTransitionSpawner.abortTransitions(widget);
			widget->get()->enterTransitionOut();
			widget->get()->finishTransitionOut();
		}
	}
    if (!mShutdown && storeHistory)
    {
		float priority = mWidgetGroups[category].mWidgets.findPriority(widget->get()->getId());
		mHistory.storePanel(widget, tDesc, category, priority);
    }
    
	//If we are transitioning out, the transition will handle removing from top level widget when it's done
	if (mTopLevelWidget && !didSpawnTransition)
	{
		mTopLevelWidget->get()->removeChild(widget);
	}
}

void GHGUIMgr::updateScreenSize(void)
{
    if (mTopLevelWidget)
    {
        mTopLevelWidget->get()->updatePosition();
    }
    else
    {
        for (auto groupIter = mWidgetGroups.begin(); groupIter != mWidgetGroups.end(); ++groupIter)
        {
            for (size_t i = 0; i < (*groupIter).mWidgets.size(); ++i) {
                (*groupIter).mWidgets[i]->get()->updatePosition();
            }
        }
    }
}

void GHGUIMgr::setCategoryActive(size_t category, bool active)
{
	if (mWidgetGroups.size() <= category)
	{
		mWidgetGroups.resize(category + 1);
	}
	if (active == mWidgetGroups[category].mIsActive)
	{
		return;
	}

	mWidgetGroups[category].mIsActive = active;
	if (active)
	{
		for (size_t i = 0; i < mWidgetGroups[category].mWidgets.size(); ++i) {
			mWidgetGroups[category].mWidgets[i]->get()->enterTransitionIn();
			mWidgetGroups[category].mWidgets[i]->get()->finishTransitionIn();
		}
	}
	else
	{
		for (size_t i = 0; i < mWidgetGroups[category].mWidgets.size(); ++i) {
			mWidgetGroups[category].mWidgets[i]->get()->enterTransitionOut();
			mWidgetGroups[category].mWidgets[i]->get()->finishTransitionOut();
		}
	}
}
