#pragma once

#include "GHUtils/GHController.h"
#include "OVR_CAPI.h"
#include <vector>

class GHEventMgr;

class GHOculusSystemController : public GHController
{
public:
	GHOculusSystemController(GHEventMgr& eventMgr);
	~GHOculusSystemController(void);

	virtual void onActivate(void);
	virtual void onDeactivate(void);
	virtual void update(void);

	const ovrSession getSession(void) const { return mSession; }
	const ovrHmdDesc& getHMDDesc(void) const { return mHMDDesc; }
	const ovrTrackingState& getTrackingState(void) const { return mTrackingState; }
	const ovrEyeRenderDesc& getEyeRenderDesc(ovrEyeType eyeType) const { return mEyeRenderDesc[eyeType]; }
	const ovrPosef& getEyeRenderPose(ovrEyeType eyeType) const { return mEyeRenderPose[eyeType]; }

	float getOculusToGameScale(void) const;
	float getGameToOculusScale(void) const;

	bool isVisible(void) const { return mIsVisible; }

	struct InputState
	{
		//indices into the GH input state's gamepad list
		enum GamepadIndex
		{
			GI_CONTROLLER = 0,
			GI_TOUCH = 1,
			GI_REMOTE = 2,
			GI_NUMINPUTSTATES
		};

		ovrInputState mOvrInput;
		ovrControllerType mControllerType;
		GamepadIndex mGamepadIndex;
		bool mIsActive { false };

		InputState(ovrControllerType controllerType, GamepadIndex gamepadIndex) 
			: mControllerType(controllerType), mGamepadIndex(gamepadIndex)
		{ 
		
		}
	};

	const InputState& getInputState(InputState::GamepadIndex gamepadIndex) const;

private:
	InputState					mInputStates[InputState::GI_NUMINPUTSTATES];

	GHEventMgr&					mEventMgr;
	ovrSession					mSession { nullptr };
	ovrHmdDesc					mHMDDesc;
	std::vector<ovrTrackerDesc>	mTrackerDescs;
	ovrTrackingState			mTrackingState;
	ovrEyeRenderDesc			mEyeRenderDesc[ovrEye_Count];
	ovrPosef					mEyeRenderPose[ovrEye_Count];
	bool						mIsVisible{ false };
};
