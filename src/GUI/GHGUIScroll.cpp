// Copyright Golden Hammer Software
#include "GHGUIScroll.h"
#include "GHGUIWidget.h"
#include "GHPlatform/GHDebugMessage.h"
#include "GHScreenInfo.h"
#include "GHMath/GHFloat.h"
#include "GHPlatform/GHTimeVal.h"
#include <algorithm>
#include "GHInputState.h"
#include "GHKeyDef.h"
#include "GHWidgetStates.h"
#include "GHGUIRectMaker.h"
#include "GHGUICanvas.h"
#include "GHPointerRegionTracker.h"
#include "GHUtils/GHControllerMgr.h"
#include "GHGUIController.h"

GHGUIScroll::GHGUIScroll(const GHGUIPosDesc& childPosDesc,
                         GHInputState& inputState,
                         GHInputClaim& inputClaim,
                         const GHScreenInfo& screenInfo,
                         const GHTimeVal& timeval,
						 const GHGUIRectMaker& rectMaker,
						 GHControllerMgr& controllerMgr)
: mScreenInfo(screenInfo)
, mTimeVal(timeval)
, mParentWidget(0)
, mChildPosDesc(childPosDesc)
, mInputState(inputState)
, mInputClaim(inputClaim)
, mRectMaker(rectMaker)
, mControllerMgr(controllerMgr)
, mCurrentOffset(0)
, mWasSelected(false)
, mLastDiff(0)
{
}

GHGUIScroll::~GHGUIScroll(void)
{
	if (mPointerTracker) delete mPointerTracker;
}

void GHGUIScroll::setParentWidget(GHGUIWidget* parentWidget)
{
	if (mPointerTracker) 
	{
		delete mPointerTracker;
		mPointerTracker = 0;
	}

	mParentWidget = parentWidget; 
	if (parentWidget)
	{
		std::vector<int> triggerKeys;
		mPointerTracker = new GHPointerRegionTracker(mParentWidget->getScreenPos(), mInputState, mInputClaim, triggerKeys, mTriggerThreshold, true, false);
		mPointerTracker->setAllowPointerClaim(false);
	}
}

void GHGUIScroll::onActivate(void)
{
    mLastDiff = 0;
    mCurrentOffset = 0;
	mOffsetSincePointerClaimReleased = 0;
    mWasSelected = false;
    updatePositions();
}

void GHGUIScroll::onDeactivate(void)
{
    
}

void GHGUIScroll::update(void)
{
    updateInput();
    impartMomentum();
    clampOffset(); 

	if (mAllowScrolling)
	{
		updatePositions();

		int depth = mParentWidget->findDepthOfDeepestChild();
		if (depth != mLastDepth)
		{
			mControllerMgr.updatePriority(this, GHGUIController::calcPriority(depth) - 0.1f);
			mLastDepth = depth;
		}
	}

    mWasSelected = mPointerTracker->isSelected();
}

void GHGUIScroll::updatePositions(void)
{
	if (!mParentWidget) {
		return;
	}

    GHGUIPosDesc posDesc = mChildPosDesc;
    posDesc.mOffset[1] = mCurrentOffset;
    size_t numChildren = mParentWidget->getNumChildren();
	GHRect<float, 2> screenPos;
	float pixelSizeMod = mParentWidget->calcPixelSizeMod();
	IGHGUICanvas* canvas = IGHGUICanvas::get3dCanvas(mParentWidget->getPropertyContainer());
    for (size_t i = 0; i < numChildren; ++i)
    {
        GHGUIWidgetResource* child = mParentWidget->getChildAtIndex(i);
        GHGUIWidget* widget = child->get();

		// see if this child would be visible.
		mRectMaker.createRect(posDesc, screenPos, &mParentWidget->getScreenPos(), pixelSizeMod, canvas);
		bool vis = checkChildVisibility(screenPos);
		if (vis)
		{
			widget->setPosDesc(posDesc);
			widget->updatePosition();
		}
		setChildVisibility(*child, vis);

        posDesc.mOffset[1] += posDesc.mSize[1];
    }
    mParentWidget->updatePosition();
}

void GHGUIScroll::updateInput(void)
{
	// if we have a canvas then redirect our pointer tracker to look at those collisions.
	IGHGUICanvas* canvas = IGHGUICanvas::get3dCanvas(mParentWidget->getPropertyContainer());
	if (canvas)
	{
		mPointerTracker->setPointerPositionOverrides(&canvas->getPointerOverrides());
	}
	else
	{
		mPointerTracker->setPointerPositionOverrides(0);
	}
	mPointerTracker->update();

    if (mPointerTracker->isSelected()) {
        const GHPoint2& activePosition = mPointerTracker->getActivePosition();
        if (mWasSelected) 
		{
			float amountMoved = activePosition[1] - mLastYPos;
			// get rid of some jittering
			if (::fabs(amountMoved) > 0.0001f)
			{
				// interpolate the last diff to smooth out the movement.
				amountMoved = convertPixelScale(amountMoved);
				float movedDiff = amountMoved - mLastDiff;
				float timeMod = mTimeVal.getTimePassed() * 10.f;
				if (timeMod > 1.0f) timeMod = 1.0f;
				mLastDiff += movedDiff * timeMod;
				mCurrentOffset += mLastDiff;

				if (!mPointerTracker->getAllowPointerClaim() 
					&& fabs(mCurrentOffset - mOffsetSincePointerClaimReleased) > mPointerClaimThreshold)
				{
					mPointerTracker->setAllowPointerClaim(true); 
					mAllowScrolling = true;
				}

				mLastYPos = activePosition[1];
			}
			else
			{
				decayLastDiff();
			}
        }
		else
		{
			mLastYPos = activePosition[1];
		}
    }
	else
	{
		mPointerTracker->setAllowPointerClaim(false);
		mOffsetSincePointerClaimReleased = mCurrentOffset;
	}

	// handle scroll wheel
	const GHInputStructs::InputEventList& ie = mInputState.getInputEvents();
	for (auto iter = ie.begin(); iter != ie.end(); ++iter)
	{
		if ((*iter).mType != GHInputStructs::IET_KEYCHANGE) continue;
		if ((*iter).mId != 0) continue;
		if ((*iter).mVal[0] != GHKeyDef::KEY_MOUSEWHEEL) continue;

		//only allow scroll wheel if pointer 0 is over our window
		if (pointerPosInRect(mInputState.getPointerPosition(0)))
		{
			float mouseWheelDiff = mMouseWheelMultiplier * (*iter).mVal[1];
			mLastDiff += mouseWheelDiff;
			mCurrentOffset += mouseWheelDiff;
			mAllowScrolling = true;
		}
	}
}

void GHGUIScroll::impartMomentum(void)
{
    if (!mPointerTracker->isSelected()) 
	{
        //Arbitary cutoff.
        if (!GHFloat::isZero(mLastDiff, .005f)) {
			decayLastDiff();
            mCurrentOffset += mLastDiff;
        }
		else
		{
			mLastDiff = 0;
			mAllowScrolling = false;
		}
    }
}

void GHGUIScroll::decayLastDiff(void)
{
	float timeMod = mTimeVal.getTimePassed() * mMomentumDecayFactor;
	if (timeMod > 1.0f) timeMod = 1.0f;
	mLastDiff -= mLastDiff * timeMod;
}

void GHGUIScroll::clampOffset(void)
{
	float minScroll = getMinScroll();
    float maxScroll = getMaxScroll();
    mCurrentOffset = std::max(minScroll, std::min(maxScroll, mCurrentOffset));
}

float GHGUIScroll::getMinScroll(void) const
{
	if (!mParentWidget) {
		return 0;
	}

	size_t numChildren = mParentWidget->getNumChildren();
	float totalHeight = (numChildren)*mChildPosDesc.mSize[1];
	float parentHeight = getParentHeight();

	float minScroll = totalHeight > parentHeight ? -(totalHeight - parentHeight) : 0;
	return minScroll;
}

float GHGUIScroll::getMaxScroll(void) const
{
	return 0;
}

float GHGUIScroll::getCurrentScroll(void) const
{
	return mCurrentOffset;
}

void GHGUIScroll::setCurrentScroll(float val)
{
	mCurrentOffset = val;
}

float GHGUIScroll::convertPixelScale(float diff) const
{
    if (mChildPosDesc.mYFill == GHFillType::FT_PIXELS)
    {
		return diff * getParentHeight();
    }
    else
    {
        return diff;
    }
}

float GHGUIScroll::getParentHeight(void) const
{
	if (!mParentWidget) {
		return 0;
	}

    if (mChildPosDesc.mYFill == GHFillType::FT_PIXELS
        && mParentWidget->getPosDesc().mYFill == GHFillType::FT_PCT)
    {
		float pixelSizeMod = mParentWidget->calcPixelSizeMod();
		const auto& screenPos = mParentWidget->getScreenPos();
        return (screenPos.mMax[1] - screenPos.mMin[1])*mScreenInfo.getSize()[1]/mScreenInfo.getPixelSize();
    }
    else if (mChildPosDesc.mYFill == GHFillType::FT_PCT
             && mParentWidget->getPosDesc().mYFill == GHFillType::FT_PIXELS)
    {
        return mParentWidget->getPosDesc().mSize[1]/(mScreenInfo.getSize()[1]/mScreenInfo.getPixelSize());
    }
    else return mParentWidget->getPosDesc().mSize[1];

}

void GHGUIScroll::setChildVisibility(GHGUIWidgetResource& child, bool vis)
{
	if (!vis)
	{
		if (child.get()->getWidgetStates().getState() != GHWidgetState::WS_NONE)
		{
			if (child.get()->getWidgetStates().getState() == GHWidgetState::WS_ACTIVE)
			{
				child.get()->enterTransitionOut();
				child.get()->finishTransitionOut();
			}
			else if (child.get()->getWidgetStates().getState() == GHWidgetState::WS_TRANSITIONIN)
			{
				child.get()->finishTransitionIn();
				child.get()->enterTransitionOut();
				child.get()->finishTransitionOut();
			}
			else if (child.get()->getWidgetStates().getState() == GHWidgetState::WS_TRANSITIONOUT)
			{
				child.get()->finishTransitionOut();
			}
		}
		child.get()->setHidden(true);
	}
	else
	{
		child.get()->setHidden(false);
		if (mParentWidget &&			
			child.get()->getWidgetStates().getState() != mParentWidget->getWidgetStates().getState())
		{
			if (mParentWidget->getWidgetStates().getState() == GHWidgetState::WS_TRANSITIONIN)
			{
				child.get()->enterTransitionIn();
			}
			else if (mParentWidget->getWidgetStates().getState() == GHWidgetState::WS_ACTIVE)
			{
				child.get()->enterTransitionIn();
				child.get()->finishTransitionIn();
			}
		}
	}
}

bool GHGUIScroll::checkChildVisibility(const GHRect<float, 2>& childPos)
{
	if (!mParentWidget) {
		return false;
	}

	bool vis = false;
	if (mParentWidget->getScreenPos().containsPoint(childPos.mMin, EPSILON))
	{
		vis = true;
	}
	else if (mParentWidget->getScreenPos().containsPoint(childPos.mMax, EPSILON))
	{
		vis = true;
	}
	return vis;
}

bool GHGUIScroll::pointerPosInRect(const GHPoint2& pointerPos) const
{
	if (!mParentWidget) {
		return false;
	}

	return mParentWidget->getScreenPos().containsPoint(pointerPos);
}

/*
GHGUIScroll::PointerTracker::PointerTracker(const GHInputState& inputState,
	GHInputClaim& inputClaim)
: mPointerId(-1)
, mActivePosition(0, 0)
, mInputState(inputState)
, mInputClaim(inputClaim)
, mRegion(0)
{
}

void GHGUIScroll::PointerTracker::setRegion(const GHRect<float, 2>* region)
{
	mRegion = region;
}

void GHGUIScroll::PointerTracker::update(void)
{
	if (isSelected())
	{
		// if we have a selected pointer, look for exiting.
		bool lostPointer = false;
		if (!mInputState.getPointerActive(mPointerId))
		{
			lostPointer = true;
		}
		else if (!mInputState.getKeyState(0, (int)GHKeyDef::KEY_MOUSE1 + (mPointerId * 3)))
		{
			lostPointer = true;
		}
		// note that we don't care about leaving our region once we've started.
		if (lostPointer)
		{
			mInputClaim.releasePointer(mPointerId, this);
			mPointerId = -1;
			mActivePosition.setCoords(0, 0);
		}
		else
		{
			mActivePosition = mInputState.getPointerPosition(mPointerId);
			// don't let the buttons have it.
			mInputClaim.claimPointer(mPointerId, this);
		}
	}
	else
	{
		lookForNewActivePointer();
	}
}

void GHGUIScroll::PointerTracker::lookForNewActivePointer(void)
{
	// look for an active pointer.
	// active means activated over us, and then moved without deactivating.
	for (unsigned int i = 0; i < mInputState.getNumPointers(); ++i)
	{
		bool evalPointer = false;
		if (mInputState.getPointerActive(i))
		{
			if (mInputState.getKeyState(0, (int)GHKeyDef::KEY_MOUSE1 + (i * 3)))
			{
				if (mRegion && mRegion->containsPoint(mInputState.getPointerPosition(i)))
				{
					evalPointer = true;
				}
			}
		}

		if (!evalPointer) {
			// make sure it doesn't stick around in our potential list.
			
			for (auto iter = mPotentialPointers.begin(); iter < mPotentialPointers.end(); ++iter)
			{
				if ((*iter).mPointerId == i)
				{
					mPotentialPointers.erase(iter);
					break;
				}
			}
			continue;
		}

		// check to see if this is a new pointer, or if it's one already in our list.
		int ppIdx = -1;
		for (size_t j = 0; j < mPotentialPointers.size(); ++j)
		{
			if (mPotentialPointers[j].mPointerId == i)
			{
				ppIdx = (int)j;
				break;
			}
		}
		if (ppIdx == -1)
		{
			// not a pointer we are already looking at.
			// store out the activate info.
			PointerInfo pi;
			pi.mPointerId = i;
			pi.mPositionAtActivate = mInputState.getPointerPosition(i);
			mPotentialPointers.push_back(pi);
		}
		else
		{
			// we're already tracking this pointer.
			// check to see if the pointer moved enough to become the active pointer.
			GHPoint2 currPos = mInputState.getPointerPosition(i);
			currPos -= mPotentialPointers[ppIdx].mPositionAtActivate;
			float diff = currPos.length();
			const float minDiff = 0.05f;
			if (fabs(diff) > minDiff)
			{
				// accepted!
				mPointerId = i;
				mActivePosition = mInputState.getPointerPosition(i);
				mPotentialPointers.clear();
				return;
			}
		}
	}
}

bool GHGUIScroll::PointerTracker::isSelected(void) const
{
	return (mPointerId != -1);
}

const GHPoint2& GHGUIScroll::PointerTracker::getActivePosition(void) const
{
	return mActivePosition;
}
*/
