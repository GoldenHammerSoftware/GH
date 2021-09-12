// Copyright Golden Hammer Software
#pragma once

#include "GHMath/GHRect.h"
#include <vector>
#include "GHInputStructs.h"
#include "GHKeyDef.h"

class GHInputState;
class GHInputClaim;

// For a region of screen space, keeps track of 
// 1) if any pointer is active and over the region: we are selected.
// 2) if the last active pointer is over us when it deactivates, we are activated.
// 3) if any pointers are active over this region, store the normalized position of the active pointer
class GHPointerRegionTracker
{
private:
	struct SupportedKey
	{
		SupportedKey(int pointer, GHKeyDef::Enum key, int gamepad)
			: mPointer(pointer), mTrigger(key), mGamepad(gamepad)
		{}

		int mPointer{ 0 };
		GHKeyDef::Enum mTrigger{ GHKeyDef::Enum(0) };
		int mGamepad{ -1 }; // if -1 then not a gamepad trigger.
	};

public:
	GHPointerRegionTracker(const GHRect<float, 2>& region, GHInputState& inputState,
						   GHInputClaim& inputClaim,
						   const std::vector<int>& triggerKeys,
                           float triggerThreshold,
						   bool requireMouseKey, bool requirePointerClaim = true);

    void update(void);
    void onDeactivate(void);
    
    bool isSelected(void) const { return mSelected; }
    bool isActivated(void) const { return mActivated; }
    
    // return the relative position of the "active" pointer.
    // this generally corresponds to mCapturedPointers[0]
    const GHPoint2& getActivePosition(void) const { return mActivePosition; }
    
    void addTriggerKey(int key) { mTriggerKeys.push_back(key); }
	void addAllowedPointer(int pointer, GHKeyDef::Enum key) { mAllowedPointers.push_back(SupportedKey(pointer, key, -1)); }
    
	// override the positions of the pointers we would find in the input state.
	// if this is not null then use these and ignore the input state pointers.
    void setPointerPositionOverrides(const GHInputStructs::PointerList* pointers) { mPointerOverrides = pointers; }

    void setAllowPointerClaim(bool allowPointerClaim) { mAllowClaim = allowPointerClaim; }
    bool getAllowPointerClaim(void) const { return mAllowClaim; }

private:
    void checkPointerChanges(void);
    bool findAndRemovePointer(unsigned int id, std::vector<unsigned int>& list);
    bool checkPointerAllowed(unsigned int index) const;
    bool checkPointerActive(unsigned int index) const;
    bool claimIfContains(int pointerIdx, const GHPoint2& pointerPos);
	bool isAnyKey(unsigned int key) const;
	const GHPoint2& getPointerPosition(unsigned int index);

private:
    const GHRect<float,2>& mRegion;
    GHInputState& mInputState;
	GHInputClaim& mInputClaim;
    // a flag to say at least one mouse pointer is in our region.
    bool mSelected;
    // A flag to say we were activated this frame.
    bool mActivated;
    // A flag to say whether we are allowed to claim pointers
    bool mAllowClaim{ true };
    // the position of mCapturedPointers[0]
    GHPoint2 mActivePosition;
    // The amount of press that the button trigger requires in order for the pointer to be considered selected.
    float mTriggerThreshold{ 0 };
    
    // list of pointers that are over our rectangle.
    // if any of these mouse up while no other pointers are on top, we trigger.
    std::vector<unsigned int> mCapturedPointers;
    // list of keys that can be used to trigger the button
    // if any of these key up, we trigger
    std::vector<int> mTriggerKeys;
    // whether or not we require a mouse key pressed in order to consider a pointer active
	bool mRequireMouseKey{ true };
	// false means we will allow pointers that have already been claimed.
	bool mRequirePointerClaim{ true };
	// a set of pointers we use instead of whatever is on the input state.
	const GHInputStructs::PointerList* mPointerOverrides;

	// a list of supported pointers and their trigger keys.
	// if empty, we allow all pointers
	std::vector<SupportedKey> mAllowedPointers;

};
