#pragma once

#ifdef GH_OVR_GO

#include "GHInputState.h"
#include "VrApi.h"
#include "VrApi_Helpers.h"
#include "VrApi_SystemUtils.h"
#include "GHKeyDef.h"

class GHInputState;
class GHOvrGoInputState;
class GHTransform;
class GHOvrGoFrameState;

// class to manage marshalling ovr input onto to gh input state.
class GHOvrGoInputTranslator
{
public:
	GHOvrGoInputTranslator(const GHOvrGoFrameState& frameState, 
		GHInputState& ghInputState, const GHOvrGoInputState& ovrInputState, 
		const GHTransform& hmdOrigin);

	void update(void);

private:
	struct LastFrameData
	{
		GHPoint3 mPosition;
		double mAbsoluteTime{ 0 };
		bool mHasEverBeenSet{ false };
	};

private:
	void updatePose(const ovrRigidBodyPosef& inPose, GHInputStructs::Poseable& outPose);
	void updateHead(void);
	void updateHand(void);
	void extrapolateVelocity(double absoluteTime, LastFrameData& inOutLastFrameData, GHInputStructs::Poseable& outPose);
	bool convertTrackpadAxisToDpadButtons(int gamepadIndex, float normalizedAxisValue, float deadZone, GHKeyDef::Enum negativeKey, GHKeyDef::Enum positiveKey);

private:
	LastFrameData mHandLastFrameData;
	const GHOvrGoFrameState& mFrameState;
	GHInputState& mGHInputState;
	const GHOvrGoInputState& mOvrInputState;
	const GHTransform& mHMDOrigin;
};

#endif

