#ifdef GH_OVR_GO

#include "GHOvrGoInputState.h"
#include "GHOvrGoFrameState.h"
#include "GHPlatform/GHDebugMessage.h"

GHOvrGoInputState::GHOvrGoInputState(const GHOvrGoFrameState& frameState)
	: mFrameState(frameState)
{
}

GHOvrGoInputState::~GHOvrGoInputState(void)
{
	for (size_t i = 0; i < mInputDevices.size(); ++i)
	{
		delete mInputDevices[i];
	}
	mInputDevices.clear();
}

void GHOvrGoInputState::enumerateInputDevices(void)
{
	if (!mFrameState.mOvrMobile) { return; }

	for (uint32_t deviceIndex = 0; ; deviceIndex++)
	{
		ovrInputCapabilityHeader curCaps;

		if (vrapi_EnumerateInputDevices(mFrameState.mOvrMobile, deviceIndex, &curCaps) < 0)
		{
			// no more devices
			break;
		}

		// check to see if we already have this device in the list.
		if (findInputDevice(curCaps.DeviceID) < 0)
		{
			// new device, so add it to the list.
			connectNewInputDevice(curCaps);
		}
	}

	for (OvrGoInputDevice* device : mInputDevices)
	{
		device->pollInput(mFrameState.mOvrMobile, mFrameState.mPredictedDisplayTime);
	}
}

const GHOvrGoInputState::OvrGoInputDevice* GHOvrGoInputState::getDevice(GHOvrGoInputState::DeviceType type) const
{
	ovrControllerType searchingType;
	if (type == GI_CONTROLLER)
	{
		searchingType = ovrControllerType_TrackedRemote;
	}
	else
	{
		searchingType = ovrControllerType_Headset;
	}

	for (size_t i = 0; i < mInputDevices.size(); ++i)
	{
		if (mInputDevices[i]->getType() == searchingType)
		{
			return mInputDevices[i];
		}
	}
	return 0;
}

int GHOvrGoInputState::findInputDevice(const ovrDeviceID deviceId) const
{
	for (size_t i = 0; i < mInputDevices.size(); ++i)
	{
		if (mInputDevices[i]->getDeviceID() == deviceId)
		{
			return (int)i;
		}
	}
	return -1;
}

void GHOvrGoInputState::connectNewInputDevice(const ovrInputCapabilityHeader& capsHeader)
{
	if (!mFrameState.mOvrMobile) { return; }

	OvrGoInputDevice* newDevice = 0;
	if (capsHeader.Type == ovrControllerType_Headset)
	{
		ovrInputHeadsetCapabilities headsetCapabilities;
		headsetCapabilities.Header = capsHeader;
		ovrResult result = vrapi_GetInputDeviceCapabilities(mFrameState.mOvrMobile, &headsetCapabilities.Header);
		if (result == ovrSuccess)
		{
			GHDebugMessage::outputString("Connecting input headset");
			newDevice = new OvrGoDevice_Headset(headsetCapabilities);
		}
		else
		{
			GHDebugMessage::outputString("Failed to get headset device caps");
		}
	}
	else if (capsHeader.Type == ovrControllerType_TrackedRemote)
	{
		ovrInputTrackedRemoteCapabilities remoteCapabilities;
		remoteCapabilities.Header = capsHeader;
		ovrResult result = vrapi_GetInputDeviceCapabilities(mFrameState.mOvrMobile, &remoteCapabilities.Header);
		if (result == ovrSuccess)
		{
			GHDebugMessage::outputString("Connecting input remote");
			newDevice = new OvrGoDevice_TrackedRemote(remoteCapabilities);
		}
		else
		{
			GHDebugMessage::outputString("Failed to get remote device caps");
		}
	}

	if (!newDevice)
	{
		GHDebugMessage::outputString("Untracked device type detected.");
		return;
	}
	mInputDevices.push_back(newDevice);
}

GHOvrGoInputState::OvrGoDevice_TrackedRemote::OvrGoDevice_TrackedRemote(const ovrInputTrackedRemoteCapabilities& caps)
	: mCaps(caps)
{
	mTrackedRemoteInputState.Header.ControllerType = caps.Header.Type;
}

const ovrInputCapabilityHeader*	GHOvrGoInputState::OvrGoDevice_TrackedRemote::getCaps(void) const
{
	return &mCaps.Header;
}

ovrControllerType GHOvrGoInputState::OvrGoDevice_TrackedRemote::getType(void) const
{
	return mCaps.Header.Type;
}

ovrDeviceID GHOvrGoInputState::OvrGoDevice_TrackedRemote::getDeviceID(void) const
{
	return mCaps.Header.DeviceID;
}

const char* GHOvrGoInputState::OvrGoDevice_TrackedRemote::getName(void) const
{
	return "TrackedRemote";
}

const ovrTracking* GHOvrGoInputState::OvrGoDevice_TrackedRemote::getTracking(void) const
{
	return &mTracking;
}

const ovrInputStateTrackedRemote* GHOvrGoInputState::OvrGoDevice_TrackedRemote::getTrackedRemoteInputState(void) const
{
	return &mTrackedRemoteInputState;
}

const ovrInputTrackedRemoteCapabilities* GHOvrGoInputState::OvrGoDevice_TrackedRemote::getTrackedRemoteCapabilities(void) const
{
	return &mCaps;
}

void GHOvrGoInputState::OvrGoDevice_TrackedRemote::pollInput(ovrMobile* apiContext, double predictedDisplayTime)
{
	ovrResult handTrackResult = vrapi_GetInputTrackingState(apiContext, mCaps.Header.DeviceID, predictedDisplayTime, &mTracking);
	ovrResult inputStateResult = vrapi_GetCurrentInputState(apiContext, mCaps.Header.DeviceID, &mTrackedRemoteInputState.Header);

	mIsActive = handTrackResult == ovrSuccess 
			&& inputStateResult == ovrSuccess;

	if (!mIsActive)
	{
		GHDebugMessage::outputString("hand disconnected?");
	}
}

bool GHOvrGoInputState::OvrGoDevice_TrackedRemote::isActive(void) const
{
	return mIsActive;
}

GHOvrGoInputState::OvrGoDevice_Headset::OvrGoDevice_Headset(const ovrInputHeadsetCapabilities& caps)
	: mCaps(caps)
{
}

const ovrInputCapabilityHeader*	GHOvrGoInputState::OvrGoDevice_Headset::getCaps(void) const
{
	return &mCaps.Header;
}

ovrControllerType GHOvrGoInputState::OvrGoDevice_Headset::getType(void) const
{
	return mCaps.Header.Type;
}

ovrDeviceID GHOvrGoInputState::OvrGoDevice_Headset::getDeviceID(void) const
{
	return mCaps.Header.DeviceID;
}

const char* GHOvrGoInputState::OvrGoDevice_Headset::getName(void) const
{
	return "Headset";
}

const ovrTracking* GHOvrGoInputState::OvrGoDevice_Headset::getTracking(void) const
{
	return &mTracking;
}

const ovrInputStateTrackedRemote* GHOvrGoInputState::OvrGoDevice_Headset::getTrackedRemoteInputState(void) const
{
	return 0;
}

const ovrInputTrackedRemoteCapabilities* GHOvrGoInputState::OvrGoDevice_Headset::getTrackedRemoteCapabilities(void) const
{
	return 0;
}

void GHOvrGoInputState::OvrGoDevice_Headset::pollInput(ovrMobile* apiContext, double predictedDisplayTime)
{
	ovrResult hmdTrackResult = vrapi_GetInputTrackingState(apiContext, mCaps.Header.DeviceID, predictedDisplayTime, &mTracking);
	mIsActive = hmdTrackResult == ovrSuccess;
	if (!mIsActive)
	{
		GHDebugMessage::outputString("head disconnected?");
	}
}

bool GHOvrGoInputState::OvrGoDevice_Headset::isActive(void) const
{
	return mIsActive;
}

#endif