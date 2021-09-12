// Copyright Golden Hammer Software
#pragma once

#include "GHGUIPanel.h"
#include "GHString/GHIdentifier.h"
#include "GHUtils/GHController.h"
#include "GHMath/GHRect.h"
#include <vector>

class GHPropertyContainer;
class GHInputState;
class GHControllerMgr;
class GHInputClaim;

// reads input touches as a joystick, fills in those values into properties.
class GHGUIJoystick : public GHGUIPanel
{
	class JoyInputHandler : public GHController
	{
	public:
		JoyInputHandler(GHGUIJoystick& parent, GHInputState& inputState, GHInputClaim& pointerClaim,
			const GHRect<float, 2>& screenPos, float maxDiff, const std::vector<int>& allowedPointers);

		virtual void update(void);
		virtual void onActivate(void);
		virtual void onDeactivate(void);

	private:
		void handlePointerClaim(void);
		bool checkPointerAllowed(int pointerId);
		void tryActivateJoystick(int pointerId);
		void tryDeactivateJoystick(int pointerId);

	private:
		GHGUIJoystick& mParent;
		GHInputState& mInputState;
		GHInputClaim& mPointerClaim;
		const GHRect<float, 2>& mScreenPos;
		// our currently active pointer, -1 if none.
		int mActivePointer;

		GHPoint2 mJoyCenter;
		// our scale for mouse pos changes hitting 1.0f.
		float mMaxDiff;
		// pointers to allow to control this joystick.
		std::vector<int> mAllowedPointers;
	};

public:
	GHGUIJoystick(GHGUIRenderable& renderer, const GHGUIRectMaker& rectMaker,
		GHPropertyContainer& props, const GHIdentifier& activeProp,
		const GHIdentifier& xProp, const GHIdentifier& yProp,
		float maxDiff, const std::vector<int>& allowedPointers,
		GHInputState& inputState, GHInputClaim& pointerClaim, GHControllerMgr& controllerMgr,
		GHGUIWidgetResource* joybg, GHGUIWidgetResource* joyfg);
	~GHGUIJoystick(void);

	void handleJoystickActivated(const GHPoint2& joyCenter);
	void handleJoystickDeactivated(void);
	void handleJoystickPosition(const GHPoint2& pos);

protected:
	virtual void onFinishTransitionIn(void);
	virtual void onEnterTransitionOut(void);

	// convert a mouse position to a pct of ourselves.
	GHPoint2 convertScreenToLocal(const GHPoint2& pos);

private:
	GHPropertyContainer& mProps;
	GHIdentifier mActiveProp; // is there an active touch?
	GHIdentifier mXProp; // prop for x joystick value.
	GHIdentifier mYProp; // prop for y joystick value.

	JoyInputHandler mInputHandler;
	GHControllerMgr& mControllerMgr;

	// background widget for when active.
	GHGUIWidgetResource* mJoyBG;
	// foreground widget for when active.
	GHGUIWidgetResource* mJoyFG;
};
