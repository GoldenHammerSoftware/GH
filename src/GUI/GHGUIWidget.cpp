// Copyright Golden Hammer Software
#include "GHGUIWidget.h"
#include "GHWidgetStates.h"
#include "GHGUIRectMaker.h"
#include <stddef.h> //for size_t
#include "GHGUICanvas.h"

GHGUIWidget::GHGUIWidget(const GHGUIRectMaker& rectMaker) 
: mParent(0)
, mRectMaker(rectMaker)
, mDepth(0)
, mPixelSizeMod(1.0)
, mParentWidget(0)
, mHidden(false)
{
}

GHGUIWidget::~GHGUIWidget(void)
{
    for (size_t i = 0; i < mChildren.size(); ++i) {
        mChildren[i]->get()->setParent(0, 0);
        mChildren[i]->release();
    }
    mChildren.clear();

	// need to call this before mChildren is deleted in case there's a gui widget transition.
	mWidgetStates.shutdown();
}

void GHGUIWidget::setParent(const GHRect<float, 2>* parent, const GHGUIWidget* parentWidget) 
{ 
	mParent = parent; 
	mParentWidget = parentWidget; 
	if (mParentWidget)
	{
		mPropertyContainer.setParentProperties(&mParentWidget->getPropertyContainer());
	}
	else
	{
		mPropertyContainer.setParentProperties(0);
	}
}

void GHGUIWidget::enterTransitionIn(void)
{
    onEnterTransitionIn();
    mWidgetStates.setState(GHWidgetState::WS_TRANSITIONIN);
    
    for (size_t i = 0; i < mChildren.size(); ++i) {
		if (!mChildren[i]->get()->isHidden())
		{
			mChildren[i]->get()->enterTransitionIn();
		}
    }
    updatePosition();
}

void GHGUIWidget::finishTransitionIn(void)
{
	// make sure we're in WS_TRANSITIONIN
	if (mWidgetStates.getState() == GHWidgetState::WS_NONE)
	{
		enterTransitionIn();
	}

    onFinishTransitionIn();
    mWidgetStates.setState(GHWidgetState::WS_ACTIVE);
    
    for (size_t i = 0; i < mChildren.size(); ++i) {
		if (!mChildren[i]->get()->isHidden())
		{
			mChildren[i]->get()->finishTransitionIn();
		}
    }
}

void GHGUIWidget::enterTransitionOut(void)
{
	// make sure we're WS_ACTIVE.
	if (mWidgetStates.getState() == GHWidgetState::WS_NONE ||
		mWidgetStates.getState() == GHWidgetState::WS_TRANSITIONOUT)
	{
		return;
	}

    onEnterTransitionOut();
    mWidgetStates.setState(GHWidgetState::WS_TRANSITIONOUT);
    
    for (size_t i = 0; i < mChildren.size(); ++i) {
		if (!mChildren[i]->get()->isHidden())
		{
			mChildren[i]->get()->enterTransitionOut();
		}
    }
}

void GHGUIWidget::finishTransitionOut(void)
{
	// make sure we're in WS_TRANSITIONOUT
	if (mWidgetStates.getState() == GHWidgetState::WS_ACTIVE)
	{
		enterTransitionOut();
	}
	else if (mWidgetStates.getState() == GHWidgetState::WS_NONE)
	{
		return;
	}

	mWidgetStates.setState(GHWidgetState::WS_NONE);
    for (size_t i = 0; i < mChildren.size(); ++i) 
	{
		if (!mChildren[i]->get()->isHidden())
		{
			mChildren[i]->get()->finishTransitionOut();
		}
    }
    onFinishTransitionOut();
}

void GHGUIWidget::updatePosition(void)
{
    float pixelSizeMod = calcPixelSizeMod();
	IGHGUICanvas* canvas = IGHGUICanvas::get3dCanvas(getPropertyContainer());
    mRectMaker.createRect(mPosDesc, mScreenPos, mParent, pixelSizeMod, canvas);
    for (size_t i = 0; i < mChildren.size(); ++i) 
	{
		if (!mChildren[i]->get()->isHidden())
		{
			mChildren[i]->get()->updatePosition();
		}
    }
}

void GHGUIWidget::setPosDesc(const GHGUIPosDesc& desc)
{
    mPosDesc = desc;
}

void GHGUIWidget::addChild(GHGUIWidgetResource* child, float priority)
{
	mChildren.add(child, priority);
	child->acquire();
    child->get()->setParent(&mScreenPos, this);

    if (mWidgetStates.getState() == GHWidgetState::WS_TRANSITIONIN) {
        child->get()->enterTransitionIn();
    }
    else if (mWidgetStates.getState() == GHWidgetState::WS_ACTIVE) {
        child->get()->enterTransitionIn();
        child->get()->finishTransitionIn();
    }
    else if (mWidgetStates.getState() == GHWidgetState::WS_TRANSITIONOUT) {
        child->get()->enterTransitionIn();
        child->get()->finishTransitionIn();
        child->get()->enterTransitionOut();
    }
}

void GHGUIWidget::removeChild(GHGUIWidgetResource* child)
{
	if (mChildren.remove(child)) 
	{
		if (!child->get()->isHidden())
		{
			if (mWidgetStates.getState() == GHWidgetState::WS_ACTIVE) {
				child->get()->enterTransitionOut();
				child->get()->finishTransitionOut();
			}
			else if (mWidgetStates.getState() == GHWidgetState::WS_TRANSITIONOUT) {
				child->get()->finishTransitionOut();
			}
			else if (mWidgetStates.getState() == GHWidgetState::WS_TRANSITIONIN) {
				child->get()->finishTransitionIn();
				child->get()->enterTransitionOut();
				child->get()->finishTransitionOut();
			}
			else {
				int brkpt = 1;
				brkpt++;
			}
		}

        child->get()->setParent(0, 0);
		child->get()->setHidden(false);
        child->release();
	}
}

GHGUIWidgetResource* GHGUIWidget::getChild(const GHIdentifier& id)
{
	return mChildren.find(id);
}

const GHGUIWidgetResource* GHGUIWidget::getChild(const GHIdentifier& id) const
{
	return mChildren.find(id);
}

size_t GHGUIWidget::getNumChildren(void) const
{
    return mChildren.size();
}

GHResourcePtr<GHGUIWidget>* GHGUIWidget::getChildAtIndex(size_t index)
{
    assert (index < mChildren.size());
    return mChildren[index];
}

const GHResourcePtr<GHGUIWidget>* GHGUIWidget::getChildAtIndex(size_t index) const
{
	assert(index < mChildren.size());
	return mChildren[index];
}

int GHGUIWidget::calcDepth(int prevDepth)
{
    if (mDepth != prevDepth+1)
    {
        mDepth = prevDepth+1;
        onDepthChange();
    }
    int retDepth = mDepth;
    for (size_t i = 0; i < mChildren.size(); ++i)
    {
        retDepth = mChildren[i]->get()->calcDepth(retDepth);
    }
    return retDepth;
}

float GHGUIWidget::calcPixelSizeMod(void) const
{
    if (!mParentWidget) return mPixelSizeMod;
    return mPixelSizeMod * mParentWidget->calcPixelSizeMod();
}

int GHGUIWidget::findDepthOfDeepestChild(void) const
{
	if (!mChildren.size())
	{
		return mDepth;
	}
	return mChildren[mChildren.size() - 1]->get()->findDepthOfDeepestChild();
}

