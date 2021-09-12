// Copyright 2010 Golden Hammer Software
#pragma once

#include "GHKeyDef.h"
#include "GHMath/GHPoint.h"
#include "GHUtils/GHControllerMgr.h"
#include "GHInputState.h"

class GHApp;
class GHAppCreator;
class GHThread;
class GHMessageQueue;
class GHEventMgr;
class GHIAPStoreFactory;
class GHMutex;
class GHInputState;
class GHRenderServices;
class GHSystemServices;
class GHGameServices;
class GHScreenInfo;
class GHMessageHandler;
class GHAppRunner;

// Class to initialize the basic GHEngine stuff, and an interface to create a GHApp.
// We expect games that use GHMetroView to replace GHSpecificAppLauncher.cpp and .h
// This class should not be created until a CoreWindow exists.
class GHMetroAppLauncher
{
public:
	GHMetroAppLauncher(Windows::UI::Xaml::Controls::SwapChainBackgroundPanel^ swapChainBG);
	virtual ~GHMetroAppLauncher(void);

	void initialize(void);
	void runFrame(void);

	// claim/release the inputstate mutex.
	GHInputState& claimInputState(void);
	void releaseInputState(void);

	GHScreenInfo& getScreenInfo(void);
	GHMessageQueue& getMessageQueue(void);
	GHSystemServices* getSystemServices(void) { return mSystemServices; }

	void handleWindowSizeChanged(const GHPoint2i& newSize);
	virtual void handleOptionsMenuInvoked(void) = 0;

protected:
	virtual GHApp* createApp(void) = 0;

private:
	void pollMouseButtons(void);
	void pollButtonState(Windows::System::VirtualKey windowsKey, GHKeyDef::Enum ghKey);
	void pollAccelerometer(Windows::Devices::Sensors::Accelerometer^ accelerometer);

	ref class DeviceListener sealed
	{
	internal:
		DeviceListener(GHMessageHandler& messageHandler);

	private:
		GHMessageHandler& mMessageHandler;
		void OnDefaultAudioRenderDeviceChanged(Platform::Object ^sender, Windows::Media::Devices::DefaultAudioRenderDeviceChangedEventArgs ^args);
	};
	

protected:
	GHApp* mApp;
	GHMessageQueue* mAppMessageQueue;
    GHRenderServices* mRenderServices;
    GHSystemServices* mSystemServices;
    GHGameServices* mGameServices;
	GHControllerMgr mSystemControllerMgr;

	GHAppRunner* mAppRunner;
	GHThread* mAppThread;
	// input state for handling events from the OS thread.
	GHInputState mVolatileInputState;
	GHMutex* mInputMutex;

	// we get an error every time we query for the accelerometer,
	// so we store it to avoid output spam.
	Windows::Devices::Sensors::Accelerometer^ mAccelerometer;

	DeviceListener^ mDeviceListener;
};

