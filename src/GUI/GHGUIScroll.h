// Copyright Golden Hammer Software
#pragma once

#include "GHUtils/GHController.h"
#include <vector>
#include "GHGUIPosDesc.h"
#include "GHMath/GHRect.h"
#include "GHGUIWidget.h"
class GHInputState;
class GHInputClaim;
class GHScreenInfo;
class GHTimeVal;
class GHPointerRegionTracker;
class GHControllerMgr;

//Handles user input to scroll the children of a target GUI.
// widgets completely outside of the parent region are hidden.
class GHGUIScroll : public GHController
{
public:
	GHGUIScroll(const GHGUIPosDesc& childPosDesc,
		GHInputState& inputState,
		GHInputClaim& inputClaim,
		const GHScreenInfo& screenInfo,
		const GHTimeVal& timeVal,
		const GHGUIRectMaker& rectMaker,
		GHControllerMgr& controllerMgr);

	~GHGUIScroll(void);

	virtual void onActivate(void);
	virtual void onDeactivate(void);
	virtual void update(void);

	void setParentWidget(GHGUIWidget* parentWidget);

	void updatePositions(void);

	float getMinScroll(void) const;
	float getMaxScroll(void) const;
	float getCurrentScroll(void) const;
	void setCurrentScroll(float val);

	void setTriggerThreshold(float triggerThreshold) { mTriggerThreshold = triggerThreshold; }
	void setPointerClaimThreshold(float pointerClaimThreshold) { mPointerClaimThreshold = pointerClaimThreshold; }
	void setMomentumDecayFactor(float momentumDecayFactor) { mMomentumDecayFactor = momentumDecayFactor; }
	void setMouseWheelMultiplier(float mouseWheelMultiplier) { mMouseWheelMultiplier = mouseWheelMultiplier; }

private:
    void updateInput(void);
    float convertPixelScale(float diff) const;
    float getParentHeight(void) const;
	void decayLastDiff(void);
    void impartMomentum(void);
    void clampOffset(void);

	// enforce a child's visibility.
	void setChildVisibility(GHGUIWidgetResource& child, bool vis);
	// check to see if a child should be visible.
	bool checkChildVisibility(const GHRect<float, 2>& childPos);
	bool pointerPosInRect(const GHPoint2& pointerPos) const;

private:
    const GHScreenInfo& mScreenInfo;
	GHGUIWidget* mParentWidget{ 0 };
    GHGUIPosDesc mChildPosDesc;
    const GHTimeVal& mTimeVal;
	GHInputState& mInputState;
	GHInputClaim& mInputClaim;
	const GHGUIRectMaker& mRectMaker;
	GHControllerMgr& mControllerMgr;
	int mLastDepth{ 0 };
	float mTriggerThreshold{ 0 };
	float mPointerClaimThreshold{ 0 };
	float mMomentumDecayFactor{ 10 };
	float mMouseWheelMultiplier{ 0.1f };

    float mCurrentOffset;
    float mLastYPos;
    float mLastDiff;
	float mOffsetSincePointerClaimReleased;
	bool mWasSelected;
	bool mAllowScrolling{ false }; //Flag to allow some fudge factor before actually scrolling gui (while still keeping track of current offset)

	/*
	// keeps track of an active pointer with special scroll rules.
	// we only like pointers that started in our region, and have moved a certain amount.
	class PointerTracker
	{
	public:
		PointerTracker(const GHInputState& inputState,
					   GHInputClaim& inputClaim);

		void setRegion(const GHRect<float, 2>* region);

		void update(void);
		bool isSelected(void) const;
		const GHPoint2& getActivePosition(void) const;

		// override the positions of the pointers we would find in the input state.
		// if this is not null then use these and ignore the input state pointers.
		void setPointerPositionOverrides(const std::vector<GHPointerPositionOverride>* pointers) { mPointerOverrides = pointers; }

	private:
		// we don't have an active pointer, look to see if any qualify.
		void lookForNewActivePointer(void);

	private:
		const GHRect<float, 2>* mRegion;
		const GHInputState& mInputState;
		GHInputClaim& mInputClaim;

		// current active pointer, -1 if none
		int mPointerId;
		// the position of the active pointer.
		GHPoint2 mActivePosition;

		struct PointerInfo
		{
			int mPointerId;
			GHPoint2 mPositionAtActivate;
		};
		// the pointers we are currently looking at for becoming potentially active.
		std::vector<PointerInfo> mPotentialPointers;

		// a set of pointers we use instead of whatever is on the input state.
		const std::vector<GHPointerPositionOverride>* mPointerOverrides;
	};
	PointerTracker mPointerTracker;
	*/
	GHPointerRegionTracker* mPointerTracker{ 0 };
};
