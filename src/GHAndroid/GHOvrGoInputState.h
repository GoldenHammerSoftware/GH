#pragma once

#ifdef GH_OVR_GO

#include "VrApi.h"
#include "VrApi_Helpers.h"
#include "VrApi_SystemUtils.h"
#include "VrApi_Input.h"
#include <vector>

class GHOvrGoFrameState;

// class to hold the untranslated input from ovr go before converting it to gh.
class GHOvrGoInputState
{
public:
	class OvrGoInputDevice
	{
	public:
		virtual ~OvrGoInputDevice(void) = default;

		virtual const ovrInputCapabilityHeader*	getCaps(void) const = 0;
		virtual ovrControllerType getType(void) const = 0;
		virtual ovrDeviceID getDeviceID(void) const = 0;
		virtual const char* getName(void) const = 0;

		virtual const ovrTracking* getTracking(void) const = 0;
		virtual const ovrInputStateTrackedRemote* getTrackedRemoteInputState(void) const = 0;
		virtual const ovrInputTrackedRemoteCapabilities* getTrackedRemoteCapabilities(void) const = 0;

		virtual void pollInput(ovrMobile* apiContext, double predictedDisplayTime) = 0;
		virtual bool isActive(void) const = 0;
	};

	enum DeviceType
	{
		GI_CONTROLLER = 0,
		GI_HEADSET,
		GI_NUMDEVICETYPES
	};

public:
	GHOvrGoInputState(const GHOvrGoFrameState& frameState);
	~GHOvrGoInputState(void);

	void enumerateInputDevices(void);
	// find the first matching active device of type.
	const GHOvrGoInputState::OvrGoInputDevice* getDevice(GHOvrGoInputState::DeviceType type) const;

private:
	int findInputDevice(const ovrDeviceID deviceId) const;
	void connectNewInputDevice(const ovrInputCapabilityHeader& capsHeader);

private:
	class OvrGoDevice_TrackedRemote : public OvrGoInputDevice
	{
	public:
		OvrGoDevice_TrackedRemote(const ovrInputTrackedRemoteCapabilities& caps);

		virtual const ovrInputCapabilityHeader*	getCaps() const;
		virtual ovrControllerType getType(void) const;
		virtual ovrDeviceID getDeviceID(void) const;
		virtual const char* getName(void) const;

		virtual const ovrTracking* getTracking(void) const;
		virtual const ovrInputStateTrackedRemote* getTrackedRemoteInputState(void) const;
		virtual const ovrInputTrackedRemoteCapabilities* getTrackedRemoteCapabilities(void) const;
		virtual bool isActive(void) const;

		virtual void pollInput(ovrMobile* apiContext, double predictedDisplayTime);
	private:
		ovrInputTrackedRemoteCapabilities mCaps;
		ovrInputStateTrackedRemote mTrackedRemoteInputState;
		ovrTracking mTracking;
		bool mIsActive{ false };
	};
	class OvrGoDevice_Headset : public OvrGoInputDevice
	{
	public:
		OvrGoDevice_Headset(const ovrInputHeadsetCapabilities& caps);

		virtual const ovrInputCapabilityHeader*	getCaps(void) const;
		virtual ovrControllerType getType(void) const;
		virtual ovrDeviceID getDeviceID(void) const;
		virtual const char* getName(void) const;

		virtual const ovrTracking* getTracking(void) const;
		virtual const ovrInputStateTrackedRemote* getTrackedRemoteInputState(void) const;
		virtual const ovrInputTrackedRemoteCapabilities* getTrackedRemoteCapabilities(void) const;
		virtual bool isActive(void) const;

		virtual void pollInput(ovrMobile* apiContext, double predictedDisplayTime);
	private:
		ovrInputHeadsetCapabilities mCaps;
		ovrTracking mTracking;
		bool mIsActive{ false };
	};
	// todo? gamepad, ribbon.

private:
	const GHOvrGoFrameState& mFrameState;
	std::vector<OvrGoInputDevice*> mInputDevices;
};

#endif