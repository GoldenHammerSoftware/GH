// Copyright Golden Hammer Software
#include "GHGUITransitionController.h"
#include "GHPlatform/GHTimeVal.h"
#include "GHUtils/GHControllerMgr.h"
#include "GHUtils/GHPropertyContainer.h"
#include <assert.h>
#include "GHGUITransitionControllerFactory.h"

GHGUITransitionController::GHGUITransitionController(GHControllerMgr& controllerMgr,
                                                     const GHTimeVal& timeVal,
                                                     GHGUIWidgetResource* widget,
                                                     GHGUIWidgetResource* parentWidget,
                                                     GHPropertyContainer& props,
                                                     const GHGUITransitionDesc& tDesc,
                                                     bool activating,
                                                     GHGUITransitionControllerFactory& factory)
: mControllerMgr(controllerMgr)
, mTimeVal(timeVal)
, mWidget(0)
, mParentWidget(0)
, mDesc(tDesc)
, mProps(props)
, mActivating(activating)
, mStartTime(0)
, mWaitFinished(false)
, mFactory(factory)
{
    assert(widget != 0);
    GHRefCounted::changePointer((GHRefCounted*&)mWidget, widget);
    GHRefCounted::changePointer((GHRefCounted*&)mParentWidget, parentWidget);
}

GHGUITransitionController::~GHGUITransitionController(void)
{
    GHRefCounted::changePointer((GHRefCounted*&)mParentWidget, 0);
    GHRefCounted::changePointer((GHRefCounted*&)mWidget, 0);
    mFactory.unregister(this);
}

void GHGUITransitionController::update(void)
{
    if (!mWaitFinished) {
        if (mTimeVal.getTime() - mStartTime > mDesc.mTransitionWait)
        {
            mWaitFinished = true;
			mStartTime = mTimeVal.getTime();
			beginTransitionAnimation();
        }
        return;
    }
    if (mTimeVal.getTime() - mStartTime > mDesc.mTransitionTime)
    {
		endTransitionAnimation();
        delete this;
        return;
    }
    float pct = (mTimeVal.getTime() - mStartTime) / mDesc.mTransitionTime;
    if (!mActivating) pct = 1.0f - pct;
    mProps.setProperty(mDesc.mTransitionProp, pct);
}

void GHGUITransitionController::onActivate(void)
{
    mStartTime = mTimeVal.getTime();
    if (mActivating) mProps.setProperty(mDesc.mTransitionProp, 0.0f);
    else mProps.setProperty(mDesc.mTransitionProp, 1.0f);
    mWaitFinished = false;
}

void GHGUITransitionController::onDeactivate(void)
{
    if (mActivating) mProps.setProperty(mDesc.mTransitionProp, 1.0f);
    else mProps.setProperty(mDesc.mTransitionProp, 0.0f);
}

void GHGUITransitionController::beginTransitionAnimation(void)
{
	if (mActivating) {
		mWidget->get()->enterTransitionIn();
		mProps.setProperty(mDesc.mTransitionProp, 0);
	}
	else {
		mWidget->get()->enterTransitionOut();
		mProps.setProperty(mDesc.mTransitionProp, 1);
	}
}

void GHGUITransitionController::endTransitionAnimation(void)
{
	if (mActivating) {
		mProps.setProperty(mDesc.mTransitionProp, 1.0);
		mWidget->get()->finishTransitionIn();
	}
	else {
		mProps.setProperty(mDesc.mTransitionProp, 0.0);
		mWidget->get()->finishTransitionOut();
        if (mParentWidget && mParentWidget->get()) {
            mParentWidget->get()->removeChild(mWidget);
        }
	}
	mControllerMgr.remove(this);
}
