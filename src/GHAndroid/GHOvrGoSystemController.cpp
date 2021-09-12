#ifdef GH_OVR_GO

#include "GHOvrGoSystemController.h"
#include "GHOvrGoFrameState.h"
#include "GHInputState.h"
#include "GHPlatform/GHDebugMessage.h"

GHOvrGoSystemController::GHOvrGoSystemController(GHOvrGoFrameState& frameState, GHInputState& inputState, const GHTransform& hmdOrigin)
	: mFrameState(frameState)
	, mGHInputState(inputState)
	, mOvrInputState(frameState)
	, mInputTranslator(frameState, inputState, mOvrInputState, hmdOrigin)
{
}

GHOvrGoSystemController::~GHOvrGoSystemController(void)
{
}

void GHOvrGoSystemController::onActivate(void)
{
}

void GHOvrGoSystemController::onDeactivate(void)
{
}

void GHOvrGoSystemController::update(void)
{
	updateOvrFrameState();
	pollInput();
}

void GHOvrGoSystemController::updateOvrFrameState(void)
{
	// maybe move this code into frame state?

	if (!mFrameState.mOvrMobile)
	{
		mFrameState.mIsTrackingReady = false;
		return;
	}

	mFrameState.mFrameId++;
	mFrameState.mPredictedDisplayTime = vrapi_GetPredictedDisplayTime(mFrameState.mOvrMobile, mFrameState.mFrameId);
	mFrameState.mPredictedTracking = vrapi_GetPredictedTracking2(mFrameState.mOvrMobile, mFrameState.mPredictedDisplayTime);
	mFrameState.mIsTrackingReady = mFrameState.mPredictedTracking.Status & VRAPI_TRACKING_STATUS_ORIENTATION_TRACKED;
}

void GHOvrGoSystemController::pollInput(void)
{
	mOvrInputState.enumerateInputDevices();
	mInputTranslator.update();
}

#endif