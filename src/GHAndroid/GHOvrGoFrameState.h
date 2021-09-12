#pragma once

#ifdef GH_OVR_GO

#include "VrApi.h"
#include "VrApi_Helpers.h"
#include "VrApi_SystemUtils.h"

// the info we need from ovr for a given frame.
class GHOvrGoFrameState
{
public:
	float getOculusToGameScale(void) const
	{
		const float kGameScaleToMeters = 1.0f;
		const float kMetersToGameScale = 1.0f / kGameScaleToMeters;
		return kMetersToGameScale;
	}
	float getFloorOffset(void) const { return 1.7f; }

public:
	ovrMobile* mOvrMobile{ nullptr };
	ovrTracking2 mPredictedTracking;
	double mPredictedDisplayTime{ 0 };
	bool mIsTrackingReady{ false };
	// over expects a frame id that increments.
	unsigned int mFrameId{ 1 };
};

#endif