#pragma once
#include "GHUtils/GHController.h"
#include <openvr.h>
#include "GHInputState.h"
#include "GHKeyDef.h"

//#define GH_OPENVR_DEBUG_HANDS 1
#ifdef GH_OPENVR_DEBUG_HANDS
#include "GHDebugDraw.h"
#endif

class GHOpenVRSystem;
class GHTimeVal;
class GHTransform;

class GHOpenVRInputHandler : public GHController
{
public:
	GHOpenVRInputHandler(const GHTransform& hmdOrigin,
						 GHOpenVRSystem& system,
						 GHInputState& inputState,
						 const GHTimeVal& timeVal);

	virtual void onActivate(void);
	virtual void onDeactivate(void);
	virtual void update(void);

	const vr::TrackedDevicePose_t& getCurrentHeadPose(void) const;

private:
	void updateImpliedActions(void);
	void updatePose(const vr::TrackedDevicePose_t& inPose, GHInputStructs::Poseable& outPose) const;
	void rotateHandPose(GHTransform& handTransform) const;
	void updatePoseables(void);

private:
	vr::TrackedDevicePose_t mTrackedDevicePose[vr::k_unMaxTrackedDeviceCount];
	const GHTransform& mHMDOrigin;
	GHOpenVRSystem& mVRSystem;
	GHInputState& mInputState;
	const GHTimeVal& mTimeVal;

#ifdef GH_OPENVR_DEBUG_HANDS
	DebugCross mDebugCross[2];
#endif

	struct VrToGHActionMapping
	{
		enum ActionType
		{
			AT_BUTTON,
			AT_SKELETON,
			AT_JOYSTICK
		};

		VrToGHActionMapping(const char* name, GHKeyDef::Enum key, ActionType type, GHInputStructs::Gamepad::JoyIndex joyIndex = GHInputStructs::Gamepad::LEFT);

		const char* mActionName{ 0 };
		GHKeyDef::Enum mGHKey;
		GHInputStructs::Gamepad::JoyIndex mJoyIndex{ GHInputStructs::Gamepad::LEFT };
		vr::VRActionHandle_t mVRAction;
		ActionType mActionType;
	};
	class ActionManager
	{
	public:
		ActionManager(void);

		void updateActions(GHInputState& inputState);

	private:
		bool getDigitalActionState(vr::VRActionHandle_t action, vr::VRInputValueHandle_t* pDevicePath = nullptr);
		bool getAnalogActionState(vr::VRActionHandle_t action, GHPoint3& outPos);
		void updateSkeletonAction(const VrToGHActionMapping& action);

	private:
		vr::VRActionSetHandle_t mActionSetMain{ vr::k_ulInvalidActionSetHandle };
		std::vector<VrToGHActionMapping> mActions;
	};
	ActionManager mActionManager;
};