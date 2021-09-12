#pragma once

#include "GHUtils/GHController.h"
#include "GHString/GHIdentifier.h"
#include "GHUtils/GHMessageHandler.h"
#include "GHMath/GHTransform.h"

class GHPropertyContainer;
class GHInputState;
class GHTimeVal;
class GHEventMgr;

//Handles recentering the HMD origin (passed in by referenece as cameraCenter)
// via user input. Also handles the accept and cancel messages.
class GHHMDRecenterController : public GHController
{
public:
	GHHMDRecenterController(const GHTimeVal& timeVal,
						   GHPropertyContainer& globalProps, 
						   const GHIdentifier& hmdCenterId, 
						   const GHIdentifier& hmdCenterDefaultId,
						   const GHIdentifier& optionsSaveMessageId,
						   GHTransform& cameraCenter,
						   const GHInputState& inputState,
						   GHEventMgr& eventMgr,
						   unsigned int gamepadIndex);

	
	virtual void onActivate(void);
	virtual void onDeactivate(void);
	virtual void update(void);

private:
	void implementaitonWithJoysticks(void);
	void implementationWithHMDPos(void);

	class ConfirmationHandler : public GHMessageHandler
	{
	public:
		ConfirmationHandler(GHEventMgr& eventMgr, GHPropertyContainer& props,
							const GHIdentifier& hmdCenterId,
							const GHIdentifier& hmdCenterDefaultId, 
							const GHIdentifier& optionsSaveMessageId,
							GHTransform& cameraCenter);
		~ConfirmationHandler();
		void onActivate(void);
		void onDeactivate(void);
		virtual void handleMessage(const GHMessage& message);
	private:
		void setHMDCenterProperty(const GHPoint16& matrix);
		const GHPoint16& getHMDCenterDefault(void);
		GHPoint16& getOrSetMatrixProperty(const GHIdentifier& propID, const GHPoint16& newMatrix, bool overrideExistingValue);

	private:
		GHTransform mBackupTransform;
		GHIdentifier mHMDCenterID;
		GHIdentifier mHMDCenterDefaultID;
		GHIdentifier mOptionsSaveMessageID;
		GHTransform& mCameraCenter;
		GHEventMgr& mEventMgr;
		GHPropertyContainer& mProps;
	} mConfirmationHandler;

private:
	GHPropertyContainer& mGlobalProps;
	const GHTimeVal& mTimeVal;
	GHTransform& mCameraCenter;
	const GHInputState& mInputState;
	unsigned int mGamepadIndex;
};
