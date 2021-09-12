// Copyright Golden Hammer Software
#include "GHPointerRegionTracker.h"
#include "GHInputState.h"
#include "GHKeyDef.h"
#include "GHPlatform/GHDebugMessage.h"

GHPointerRegionTracker::GHPointerRegionTracker(const GHRect<float, 2> & region, GHInputState& inputState,
	GHInputClaim& inputClaim,
	const std::vector<int>& triggerKeys,
	float triggerThreshold,
	bool requireMouseKey, bool requirePointerClaim)
	: mRegion(region)
	, mInputState(inputState)
	, mInputClaim(inputClaim)
	, mTriggerKeys(triggerKeys)
	, mSelected(false)
	, mActivated(false)
	, mTriggerThreshold(triggerThreshold)
	, mRequireMouseKey(requireMouseKey)
	, mRequirePointerClaim(requirePointerClaim)
	, mPointerOverrides(0)
{
	// support the mouse pointer.
	mAllowedPointers.push_back(SupportedKey(0, GHKeyDef::KEY_MOUSE1, -1));
	// index 1 is the first touch.  do not allow the second touch, otherwise we can trigger two buttons at once.
	mAllowedPointers.push_back(SupportedKey(1, GHKeyDef::Enum(GHKeyDef::KEY_MOUSE1 + 3), -1));
	for (unsigned int gamepadIdx = 0; gamepadIdx < mInputState.getNumGamepads(); ++gamepadIdx)
	{
		// support the left and right pointer.
		mAllowedPointers.push_back(SupportedKey(inputState.calcPointerIndexForGamepadPoseable(gamepadIdx, 0), GHKeyDef::KEY_GP_LEFTTRIGGER, gamepadIdx));
		mAllowedPointers.push_back(SupportedKey(inputState.calcPointerIndexForGamepadPoseable(gamepadIdx, 1), GHKeyDef::KEY_GP_RIGHTTRIGGER, gamepadIdx));
	}
}

void GHPointerRegionTracker::update(void)
{
    mSelected = false;
    mActivated = false;
    
    const GHInputStructs::InputEventList& events = mInputState.getInputEvents();
    bool mouseTriggered = false;
    bool keyTriggered = false;

    for (unsigned int i = 0; i < events.size(); ++i)
    {
        if (events[i].mType == GHInputStructs::IET_KEYCHANGE || events[i].mType == GHInputStructs::IET_GAMEPADKEYCHANGE)
        {
			// check for pointer triggers.
			for (size_t allowedIdx = 0; allowedIdx < mAllowedPointers.size(); ++allowedIdx)
			{
				if (events[i].mVal[0] == mAllowedPointers[allowedIdx].mTrigger)
				{
					if (!events[i].mVal[1])
					{
						mouseTriggered |= findAndRemovePointer(mAllowedPointers[allowedIdx].mPointer, mCapturedPointers);
					}
					else
					{
						// try to deal well with triggers oscillating between within the dead zone and not in the dead zone.
						// use a bigger press threshold for activate and a smaller press threshold for deactivate.
						// this avoids oscillating between pressed and not pressed when very close to the threshold.
						bool isCurrentlyOwned = false;
						float pressThreshold = 0.5f;
						std::vector<unsigned int>::iterator findIter;
						for (findIter = mCapturedPointers.begin(); findIter != mCapturedPointers.end(); ++findIter)
						{
							if (*findIter == mAllowedPointers[allowedIdx].mPointer) 
							{
								pressThreshold = 0.4f;
								break;
							}
						}
						if (events[i].mVal[1] > pressThreshold)
						{
							claimIfContains(mAllowedPointers[allowedIdx].mPointer, getPointerPosition(mAllowedPointers[allowedIdx].mPointer));
						}
					}
				}
			}
			// check for keypress triggers.  these don't require mouse over.
            if (events[i].mVal[1])
            {
                for (unsigned int j = 0; j < mTriggerKeys.size(); ++j) 
				{
				    bool wantTrigger = false;
				    unsigned int eventKey = (unsigned int)events[i].mVal[0];
				    unsigned int triggerKey = mTriggerKeys[j];
				    if (eventKey == triggerKey)
				    {
				        wantTrigger = true;
				    }
				    else if (triggerKey == GHKeyDef::KEY_ANY)
				    {
                        if (isAnyKey(eventKey))
                        {
							wantTrigger = true;
                        }
                    }
					if (wantTrigger)
					{
						if (mInputState.claimEvent(i, this))
						{
							keyTriggered = true;
						}
					}
                }
            }
        }
		else if (events[i].mType == GHInputStructs::IET_POINTERACTIVATE)
		{
			if (checkPointerAllowed(events[i].mId)) {
				claimIfContains(events[i].mId, mInputState.getPointerPosition(events[i].mId));
			}
		}
        else if (events[i].mType == GHInputStructs::IET_POINTERDEACTIVE)
        {
            if (checkPointerAllowed(events[i].mId)) {
                mouseTriggered |= findAndRemovePointer(events[i].mId, mCapturedPointers);
            }
        }
    }
    if (keyTriggered || (mouseTriggered && !mCapturedPointers.size()))
    {
        mActivated = true;
    }
    
    checkPointerChanges();
}

void GHPointerRegionTracker::onDeactivate(void)
{
    for (size_t i = 0; i < mCapturedPointers.size(); ++i)
    {
        mInputClaim.releasePointer(mCapturedPointers[i], this);
    }
}

void GHPointerRegionTracker::checkPointerChanges(void)
{
    for (size_t i = 0; i < mCapturedPointers.size(); ++i)
    {
        mInputClaim.releasePointer(mCapturedPointers[i], this);
    }
    mCapturedPointers.resize(0);

	bool firstCapture = true;

	const GHInputStructs::PointerList& pointerList = mPointerOverrides ? *mPointerOverrides : mInputState.getPointerList();
	for (auto pointerIter = pointerList.begin(); pointerIter != pointerList.end(); ++pointerIter)
	{
		const GHInputStructs::Pointer& pointer = *pointerIter;

		if (!pointer.mActive)
		{
			continue;
		}
		if (!claimIfContains(pointer.mPointerIndex, pointer.mPosition))
		{
			continue;
		}
		// checkPointerActive is more like checkShouldWeUseThisPointerForSelected
		// we want this check to happen after the claimIfContains.
		if (!checkPointerActive(pointer.mPointerIndex))
		{
			continue;
		}
		if (!checkPointerAllowed(pointer.mPointerIndex))
		{
			continue;
		}

		// we only pick the first capture as mActivePosition.
		if (!firstCapture)
		{
			continue;
		}
		firstCapture = false;
		mSelected = true;
		mActivePosition = pointer.mPosition;
		mActivePosition[0] = (mActivePosition[0] - mRegion.mMin[0]) / (mRegion.mMax[0] - mRegion.mMin[0]);
		mActivePosition[1] = (mActivePosition[1] - mRegion.mMin[1]) / (mRegion.mMax[1] - mRegion.mMin[1]);
	}
}

bool GHPointerRegionTracker::claimIfContains(int pointerIdx, const GHPoint2& pointerPos)
{
	if (mRegion.containsPoint(pointerPos)) 
	{
		bool inputClaimed = true;
		if (mAllowClaim) 
		{
			inputClaimed = mInputClaim.claimPointer(pointerIdx, this);
		}
		if (!mRequirePointerClaim || inputClaimed)
		{
			mCapturedPointers.push_back(pointerIdx);
			return true;
		}
	}
	return false;
}

bool GHPointerRegionTracker::findAndRemovePointer(unsigned int id, std::vector<unsigned int>& list)
{
    std::vector<unsigned int>::iterator findIter;
    for (findIter = list.begin(); findIter != list.end(); ++findIter)
    {
        if (*findIter == id) {
            mInputClaim.releasePointer(id, this);
            list.erase(findIter);
            return true;
        }
    }
    return false;
}

bool GHPointerRegionTracker::checkPointerAllowed(unsigned int index) const
{
    if (!mAllowedPointers.size()) return true;
    for (size_t i = 0; i < mAllowedPointers.size(); ++i)
    {
        if (mAllowedPointers[i].mPointer == index) return true;
    }
    return false;
}

bool GHPointerRegionTracker::checkPointerActive(unsigned int index) const
{
	// hack: pointer active isn't ready for poseable pointers, so skip that part.
	if (index < 3)
	{
		if (!mInputState.getPointerActive(index))
		{
			return false;
		}
	}
	for (size_t i = 0; i < mAllowedPointers.size(); ++i)
	{
		if (mAllowedPointers[i].mPointer == index)
		{
			if (mRequireMouseKey)
			{
				if (mAllowedPointers[i].mGamepad < 0)
				{
					if (!mInputState.getKeyState(0, mAllowedPointers[i].mTrigger))
					{
						return false;
					}
				}
				else
				{
					if (mInputState.getGamepadKeyState(mAllowedPointers[i].mGamepad, mAllowedPointers[i].mTrigger) <= mTriggerThreshold)
					{
						return false;
					}
				}
				return true;
			}
			return true;
		}
	}
    return true;
}

bool GHPointerRegionTracker::isAnyKey(unsigned int key) const
{
	return !(key >= GHKeyDef::BEGIN_TOUCH_VALS && key <= GHKeyDef::END_TOUCH_VALS);
}

const GHPoint2& GHPointerRegionTracker::getPointerPosition(unsigned int index)
{
	static const GHPoint2& defaultPos = GHPoint2(0.0f, 0.0f);

	const GHInputStructs::PointerList& pointerList = mPointerOverrides ? *mPointerOverrides : mInputState.getPointerList();
	for (auto iter = pointerList.begin(); iter != pointerList.end(); ++iter)
	{
		if (iter->mPointerIndex == index)
		{
			return iter->mPosition;
		}
	}

	return defaultPos;
}

