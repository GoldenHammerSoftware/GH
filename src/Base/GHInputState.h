// Copyright Golden Hammer Software
#pragma once

#include "GHMath/GHPoint.h"
#include <vector>
#include "GHInputClaimConcrete.h"
#include "GHInputStateKeySet.h"
#include "GHMath/GHTransform.h"
#include "GHInputStructs.h"

// A class to wrap the state of any input devices.
// Users should test to see if a piece of the state is available by calling
//  functions claimPointer etc, which will return false if someone else has already claimed them.
class GHInputState
{
public:
    GHInputState(unsigned int numPointers, 
                 unsigned int numKeySets,
                 unsigned int numAccelerometers,
				 unsigned int numGamepads,
				 unsigned int numJoysPerGamepad,
				 unsigned int numPoseablesPerGamepad,
				 unsigned int numPoseables);
    
    const GHPoint2& getPointerPosition(unsigned int index) const;
    bool getPointerActive(unsigned int index) const;
    unsigned int getNumPointers(void) const { return (unsigned int)mPointers.size(); }
	const GHInputStructs::PointerList& getPointerList(void) const { return mPointers; }

	unsigned int getNumPoseables(void) const { return (unsigned int)mPoseables.size(); }

	GHInputClaim& getInputClaim(void) { return mInputClaim; }
    
    const GHPoint3& getAccelerometer(unsigned int index) const;
    unsigned int getNumAccelerometers(void) const { return (unsigned int)mAccelerometers.size(); }
	unsigned int getNumMouseButtons(void) const { return 3; }

    const GHInputStructs::InputEventList& getInputEvents(void) const;
    bool claimEvent(unsigned int eventIndex, void* owner);

    void handlePointerChange(unsigned int index, const GHPoint2& pos);
    void handlePointerDelta(unsigned int index, const GHPoint2& delta);
    void handlePointerActive(unsigned int index, bool active);
    void handleAccelerometer(unsigned int index, const GHPoint3 vel);
	void handleKeyChange(unsigned int index, unsigned int key, float val);

	void handleGamepadJoystickChange(unsigned int gamepadIndex, unsigned int joyIndex, const GHPoint2& pos);
	void handleGamepadButtonChange(unsigned int gamepadIndex, unsigned int key, float val);
	void handleGamepadActive(unsigned int index, bool active, GHInputStructs::Gamepad::Type type = GHInputStructs::Gamepad::GPT_UNKNOWN);
	// handle collisions between multiple gamepad fillers (ie xinput and oculus) by having one claim gamepads.
	void setGamepadOwner(unsigned int gamepadIndex, void* owner);
	void* getGamepadOwner(unsigned int gamepadIndex) const;

	void handlePoseableActive(unsigned int index, bool active);
	GHInputStructs::Gamepad& getGamepadForUpdate(unsigned int index);
	GHInputStructs::Poseable& getPoseableForUpdate(unsigned int index);
	
	unsigned int getNumKeySets(void) const { return (unsigned int)mKeySets.size(); }
	const float getKeyState(unsigned int index, unsigned int key) const;
	const GHInputStateKeySet& getKeySet(unsigned int index) const;
    // check to see if a key up/down is in input events.
    bool checkKeyChange(unsigned int index, unsigned int key, bool pressed) const;

	unsigned int getNumGamepads(void) const { return (int)mGamepads.size(); }
	const GHInputStructs::Gamepad& getGamepad(unsigned int index) const;
	const float getGamepadKeyState(unsigned int index, unsigned int key) const;
	bool checkGamepadKeyChange(unsigned int gamepadIndex, unsigned int key, bool pressed) const;

	const GHInputStructs::Poseable& getPoseable(unsigned int index) const;

	void clearInputEvents(void);
    void copyValues(const GHInputState& other);

	// Calculate the magic pointer index for a gamepad poseable.
	// This allows us to treat the poseable as any other pointer for gui work.
	// It assigns the gamepad poseables pointer indices that won't collide with each other or the actual pointers.
	unsigned int calcPointerIndexForGamepadPoseable(unsigned int gamepadIdx, unsigned int poseableIdx) const;

private:    
	GHInputStructs::PointerList mPointers;
    std::vector<GHPoint3> mAccelerometers;
	std::vector<GHInputStateKeySet> mKeySets;
	std::vector<GHInputStructs::Gamepad> mGamepads;
	std::vector<void*> mGamepadOwners;
	std::vector<GHInputStructs::Poseable> mPoseables;
	GHInputStructs::InputEventList mInputEvents;
	GHInputClaimConcrete mInputClaim;
};
