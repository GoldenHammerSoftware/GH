// Copyright Golden Hammer Software
#include "GHInputState.h"
#include "assert.h"
#include <stddef.h>
#include "GHPlatform/GHDebugMessage.h"

GHInputState::GHInputState(unsigned int numPointers, 
                           unsigned int numKeySets,
                           unsigned int numAccelerometers,
						   unsigned int numGamepads,
						   unsigned int numJoysPerGamepad,
						   unsigned int numPoseablesPerGamepad,
						   unsigned int numPoseables)
{
    mPointers.resize(numPointers);
    for (size_t i = 0; i < numPointers; ++i)
    {
        mPointers[i].mActive = false;
		mPointers[i].mPointerIndex = (unsigned int)i;
    }
    mKeySets.resize(numKeySets);
    mAccelerometers.resize(numAccelerometers);
	for (size_t i = 0; i < numAccelerometers; ++i)
	{
		mAccelerometers[i].setCoords(0, 0, 0);
	}
	mGamepads.resize(numGamepads);
	mGamepadOwners.resize(numGamepads);
	for (size_t i = 0; i < numGamepads; ++i)
	{
		mGamepads[i].mJoySticks.resize(numJoysPerGamepad);
		for (size_t j = 0; j < numJoysPerGamepad; ++j)
		{
			mGamepads[i].mJoySticks[j].setCoords(0,0);
		}

		mGamepads[i].mPoseables.resize(numPoseablesPerGamepad);
		mGamepadOwners[i] = 0;
	}
	mPoseables.resize(numPoseables);
}

const GHPoint2& GHInputState::getPointerPosition(unsigned int index) const
{
    assert(index < mPointers.size());
    return mPointers[index].mPosition;
}

bool GHInputState::getPointerActive(unsigned int index) const
{
	if (index >= mPointers.size())
	{
		return false;
	}
    return mPointers[index].mActive;
}

const GHPoint3& GHInputState::getAccelerometer(unsigned int index) const
{
    assert(index < mAccelerometers.size() && "getAccelerometer called with index out of range");
    return mAccelerometers[index];
}

const float GHInputState::getKeyState(unsigned int index, unsigned int key) const
{
    assert(index < mKeySets.size());
	return mKeySets[index].getKeyState(key);
}

const GHInputStateKeySet& GHInputState::getKeySet(unsigned int index) const
{
    assert(index < mKeySets.size());
    return mKeySets[index];
}

const GHInputStructs::InputEventList& GHInputState::getInputEvents(void) const
{
    return mInputEvents;
}

bool GHInputState::claimEvent(unsigned int eventIndex, void* owner)
{
    assert(eventIndex < (int)mInputEvents.size());
    if (mInputEvents[eventIndex].mClaim == owner) return true;
    else if (mInputEvents[eventIndex].mClaim != 0) return false;
    mInputEvents[eventIndex].mClaim = owner;
    return true;
}

bool GHInputState::checkKeyChange(unsigned int index, unsigned int key, bool pressed) const
{
	GHInputStructs::InputEventList::const_iterator eventIter;
    for (eventIter = mInputEvents.begin(); eventIter != mInputEvents.end(); ++eventIter)
    {
        if ((*eventIter).mType != GHInputStructs::IET_KEYCHANGE) continue;
        if ((*eventIter).mId != index) continue;
        if ((*eventIter).mVal[0] != key) continue;
        if (!pressed && (*eventIter).mVal[1] != 0) continue;
        if (pressed && (*eventIter).mVal[1] != 1) continue;
        return true;
    }
    return false;
}

void GHInputState::handlePointerChange(unsigned int index, const GHPoint2& pos)
{
    assert(index < mPointers.size());
    
	GHInputStructs::InputEvent event;
    event.mType = GHInputStructs::IET_POINTERMOVE;
    event.mId = index;
    event.mVal[0] = pos[0];
    event.mVal[1] = pos[1];
    mInputEvents.push_back(event);
    
    mPointers[index].mPosition = pos;
}

void GHInputState::handlePointerDelta(unsigned int index, const GHPoint2& delta)
{
    assert(index < mPointers.size());

    mPointers[index].mPosition += delta;

	GHInputStructs::InputEvent event;
    event.mType = GHInputStructs::IET_POINTERMOVE;
    event.mId = index;
    event.mVal[0] = mPointers[index].mPosition[0];
    event.mVal[1] = mPointers[index].mPosition[1];
    mInputEvents.push_back(event);
}

void GHInputState::handlePointerActive(unsigned int index, bool active)
{
    assert(index < mPointers.size());
    if (mPointers[index].mActive == active) return;
    mPointers[index].mActive = active;
    
	GHInputStructs::InputEvent event;
    if (active) event.mType = GHInputStructs::IET_POINTERACTIVATE;
    else event.mType = GHInputStructs::IET_POINTERDEACTIVE;
    event.mId = index;
    mInputEvents.push_back(event);
}

void GHInputState::handleAccelerometer(unsigned int index, const GHPoint3 vel)
{
    assert(index < mAccelerometers.size());
    if (mAccelerometers[index] == vel) return;
    
	GHInputStructs::InputEvent event;
    event.mType = GHInputStructs::IET_ACCEL;
    event.mId = index;
    event.mVal = vel;
    mInputEvents.push_back(event);
    
    mAccelerometers[index] = vel;
}

void GHInputState::handleKeyChange(unsigned int index, unsigned int key, float val)
{
    assert(index < mKeySets.size());
    if (mKeySets[index].getKeyState(key) == val) return;

	GHInputStructs::InputEvent event;
    event.mType = GHInputStructs::IET_KEYCHANGE;
    event.mId = index;
    event.mVal[0] = (float)key;
    event.mVal[1] = val;
    mInputEvents.push_back(event);
    
	mKeySets[index].setKeyState(key, val);
}

void GHInputState::clearInputEvents(void)
{
    mInputEvents.clear();
}

void GHInputState::copyValues(const GHInputState& other)
{
    for (unsigned int i = 0; i < mAccelerometers.size(); ++i)
    {
        mAccelerometers[i] = other.getAccelerometer(i);
    }
    for (unsigned int i = 0; i < mPointers.size(); ++i)
    {
        mPointers[i].mActive = other.getPointerActive(i);
        mPointers[i].mPosition = other.getPointerPosition(i);
    }
    for (unsigned int i = 0; i < mKeySets.size(); ++i)
    {
		mKeySets[i].copyValues(other.getKeySet(i));
    }
	for (unsigned int i = 0; i < mGamepads.size(); ++i)
	{
		const GHInputStructs::Gamepad& gamepad = other.getGamepad(i);
		mGamepads[i].mActive = gamepad.mActive;
		for (size_t j = 0; j < mGamepads[i].mJoySticks.size(); ++j)
		{
			mGamepads[i].mJoySticks[j] = gamepad.mJoySticks[j];
		}
		for (size_t j = 0; j < mGamepads[i].mPoseables.size(); ++j)
		{
			mGamepads[i].mPoseables[j] = gamepad.mPoseables[j];
		}
		mGamepads[i].mButtons.copyValues(gamepad.mButtons);
	}
	for (unsigned int i = 0; i < mPoseables.size(); ++i)
	{
		mPoseables[i] = other.mPoseables[i];
	}
    const GHInputStructs::InputEventList& inputEvents = other.getInputEvents();
	GHInputStructs::InputEventList::const_iterator ieIter;
    for (ieIter = inputEvents.begin(); ieIter != inputEvents.end(); ++ieIter)
    {
        mInputEvents.push_back(*ieIter);
    }
}

const GHInputStructs::Gamepad& GHInputState::getGamepad(unsigned int index) const
{
	assert(index < mGamepads.size());
	return mGamepads[index];
}

const float GHInputState::getGamepadKeyState(unsigned int index, unsigned int key) const
{
	assert(index < mGamepads.size());
	return mGamepads[index].mButtons.getKeyState(key);
}

bool GHInputState::checkGamepadKeyChange(unsigned int gamepadIndex, unsigned int key, bool pressed) const
{
	assert(gamepadIndex < mGamepads.size());
	GHInputStructs::InputEventList::const_iterator eventIter;
	for (eventIter = mInputEvents.begin(); eventIter != mInputEvents.end(); ++eventIter)
	{
		if ((*eventIter).mType != GHInputStructs::IET_GAMEPADKEYCHANGE) continue;
		if ((*eventIter).mId != gamepadIndex) continue;
		if ((*eventIter).mVal[0] != key) continue;
		if (!pressed && (*eventIter).mVal[1] != 0) continue;
		if (pressed && (*eventIter).mVal[1] != 1) continue;
		return true;
	}
	return false;
}

void GHInputState::handleGamepadJoystickChange(unsigned int gamepadIndex, unsigned int joyIndex, const GHPoint2& pos)
{
	assert(gamepadIndex < mGamepads.size());
	assert(joyIndex < mGamepads[0].mJoySticks.size());

	if (mGamepads[gamepadIndex].mJoySticks[joyIndex] == pos) {
		// no change, so no input event.
		return;
	}

	GHInputStructs::InputEvent event;
	event.mType = GHInputStructs::IET_GAMEPADPOINTERCHANGE;
	event.mId = gamepadIndex;
	event.mVal[0] = pos[0];
	event.mVal[1] = pos[1];
	event.mVal[2] = (float)joyIndex;
	mInputEvents.push_back(event);

	mGamepads[gamepadIndex].mJoySticks[joyIndex] = pos;
}

void GHInputState::handleGamepadButtonChange(unsigned int gamepadIndex, unsigned int key, float val)
{
	assert(gamepadIndex < mGamepads.size());
	float oldVal = mGamepads[gamepadIndex].mButtons.getKeyState(key);
	if (oldVal == val) return;

	const float threshold = 0.7f;
	if ((oldVal > threshold) != (val > threshold))
	{
		float eventVal = 0.0f;
		if (val > threshold) eventVal = 1.0f;

		GHInputStructs::InputEvent event;
		event.mType = GHInputStructs::IET_GAMEPADKEYCHANGE;
		event.mId = gamepadIndex;
		event.mVal[0] = (float)key;
		event.mVal[1] = eventVal;
		mInputEvents.push_back(event);
	}

	mGamepads[gamepadIndex].mButtons.setKeyState(key, val);
}

void GHInputState::handleGamepadActive(unsigned int index, bool active, GHInputStructs::Gamepad::Type type)
{
	assert(index < mGamepads.size());
	mGamepads[index].mActive = active;
	mGamepads[index].mType = type;
}

void GHInputState::setGamepadOwner(unsigned int gamepadIndex, void* owner)
{
	assert(gamepadIndex < mGamepadOwners.size());
	mGamepadOwners[gamepadIndex] = owner;
}

void* GHInputState::getGamepadOwner(unsigned int gamepadIndex) const
{
	assert(gamepadIndex < mGamepadOwners.size());
	return mGamepadOwners[gamepadIndex];
}

void GHInputState::handlePoseableActive(unsigned int index, bool active)
{
	assert(index < mPoseables.size());
	mPoseables[index].mActive = active;
}

GHInputStructs::Gamepad& GHInputState::getGamepadForUpdate(unsigned int index)
{
	assert(index < mGamepads.size());
	return mGamepads[index];
}

GHInputStructs::Poseable& GHInputState::getPoseableForUpdate(unsigned int index)
{
	assert(index < mPoseables.size());
	return mPoseables[index];
}

const GHInputStructs::Poseable& GHInputState::getPoseable(unsigned int index) const
{
	assert(index < mPoseables.size());
	return mPoseables[index];
}

unsigned int GHInputState::calcPointerIndexForGamepadPoseable(unsigned int gamepadIdx, unsigned int poseableIdx) const
{
	assert(gamepadIdx < getNumGamepads());

	// arbitrary non-colliding index
	// numPointers + gamepadIdx*numPoseablesPerGamepad + poseableIdx.
	unsigned int ret = getNumPointers();
	for (unsigned int i = 0; i < gamepadIdx; ++i)
	{
		ret += (unsigned int)getGamepad(i).mPoseables.size();
	}
	ret += poseableIdx;
	return ret;
}
