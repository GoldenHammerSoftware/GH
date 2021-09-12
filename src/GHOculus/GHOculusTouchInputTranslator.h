#pragma once

#include "GHUtils/GHController.h"
#include "OVR_CAPI.h"
#include "GHInputState.h"

class GHTransform;
class GHOculusSystemController;

class GHOculusTouchInputTranslator : public GHController
{
public:
	GHOculusTouchInputTranslator(const GHTransform& hmdOrigin,
								 const GHOculusSystemController& oculusSystem,
								 GHInputState& inputState);

	virtual void onActivate(void);
	virtual void onDeactivate(void);
	virtual void update(void);

private:
	void updatePose(const ovrPoseStatef& inPose, GHInputStructs::Poseable& outPose);
	void handleButtonPress(unsigned int ghGPIndex, const ovrInputState& ovrInput, unsigned int ghKey, unsigned int ovrKey);
	void handleButtonTouch(unsigned int ghGPIndex, const ovrInputState& ovrInput, unsigned int ghKey, unsigned int ovrKey);
	void handleButton(unsigned int ghGPIndex, const unsigned int ovrBitmask, unsigned int ghKey, unsigned int ovrKey);
	void handleNonBinaryButton(unsigned int ghGPIndex, unsigned int ghKey, float ovrValue);

	void handleInputCommonToTouchAndGamepad(unsigned int ghGPIndex, const ovrInputState& ovrInput);
	void handleInputCommonToGamepadAndRemote(unsigned int ghGPIndex, const ovrInputState& ovrInput);

private:
	const GHTransform& mHMDOrigin;
	const GHOculusSystemController& mOculusSystem;
	GHInputState& mInputState;
};
