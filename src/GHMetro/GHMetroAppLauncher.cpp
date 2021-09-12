// Copyright 2010 Golden Hammer Software
#include "GHMetroAppLauncher.h"
#include "GHUtils/GHMessageQueue.h"
#include "GHUtils/GHEventMgr.h"
#include "GHPlatform/GHMutex.h"
#include "GHInputState.h"
#include "GHMetroRenderServices.h"
#include "GHGameServices.h"
#include "GHMetroSystemServices.h"
#include "GHPlatform/GHDebugMessage.h"
#include "GHApp.h"
#include "GHRenderDeviceDX11.h"
#include "GHUtils/GHMessage.h"
#include "GHMessageTypes.h"
#include "GHMetroXAMLSwapChainCreator.h"
#include "GHMetroCoreWindowSwapChainCreator.h"
#include "GHRenderDeviceFactoryDX11Native.h"
#include "GHXInputHandler.h"
#include "GHBaseIdentifiers.h"
#include "GHPlatform/GHThreadFactory.h"
#include "GHPlatform/GHMutex.h"
#include "GHAppRunner.h"

GHMetroAppLauncher::GHMetroAppLauncher(Windows::UI::Xaml::Controls::SwapChainBackgroundPanel^ swapChainBG)
	: mApp(0)
	, mAccelerometer(nullptr)
	, mInputMutex(nullptr)
	, mVolatileInputState(GHMetroSystemServices::sMaxPointers, 1, 1, 4, 2, 2, 1)
	, mAppRunner(nullptr)
	, mAppThread(nullptr)
{
	mSystemServices = new GHMetroSystemServices(mSystemControllerMgr);

	GHDX11SwapChainCreator* swapChainCreator = 0;
	if (swapChainBG == nullptr) {
		swapChainCreator = new GHMetroCoreWindowSwapChainCreator;
	}
	else {
		swapChainCreator = new GHMetroXAMLSwapChainCreator(swapChainBG);
	}
	GHRenderDeviceFactoryDX11* deviceFactory = new GHRenderDeviceFactoryDX11Native(*swapChainCreator, mSystemServices->getEventMgr(), 1);

	Windows::UI::Core::CoreWindow^ window = Windows::UI::Core::CoreWindow::GetForCurrentThread();
	assert(window);
	GHPoint2i screenSize((int)window->Bounds.Width, (int)window->Bounds.Height);

	mRenderServices = new GHMetroRenderServices(*mSystemServices, deviceFactory, swapChainCreator, screenSize);

	mAppMessageQueue = new GHMessageQueue(&mSystemServices->getPlatformServices().getThreadFactory());
	mGameServices = new GHGameServices(*mSystemServices, *mRenderServices, *mAppMessageQueue);

	mAccelerometer = Windows::Devices::Sensors::Accelerometer::GetDefault();

	mDeviceListener = ref new DeviceListener(*mAppMessageQueue);
	
	mInputMutex = mSystemServices->getPlatformServices().getThreadFactory().createMutex();
	mVolatileInputState.handlePointerActive(0, true);

	GHDebugMessage::outputString("Launching App");
};

GHMetroAppLauncher::~GHMetroAppLauncher(void)
{
	mAppRunner->prepareForShutdown();
	delete mApp;
	delete mGameServices;
	delete mAppMessageQueue;
	delete mRenderServices;
	delete mSystemServices;
	delete mInputMutex;
	delete mAppRunner;
	delete mAppThread;
}

void GHMetroAppLauncher::initialize(void)
{
	mApp = createApp();
	mAppRunner = new GHAppRunner(mApp, mSystemServices->getEventMgr(), *mAppMessageQueue, 
		mSystemServices->getInputState(), mVolatileInputState, *mInputMutex, 
		mSystemServices->getPlatformServices().getThreadFactory(), nullptr);
	mAppThread = mSystemServices->getPlatformServices().getThreadFactory().createThread(GHThread::TP_HIGH);
	mAppThread->runThread(*mAppRunner);
}

void GHMetroAppLauncher::runFrame(void)
{
	if (!mApp) return;

	GHInputState& inputState = claimInputState();
	GHXInputHandler::pollGamepads(inputState);
	pollMouseButtons();
	pollAccelerometer(mAccelerometer);
	releaseInputState();

	mSystemControllerMgr.update();
}

GHInputState& GHMetroAppLauncher::claimInputState(void)
{
	if (mInputMutex) mInputMutex->acquire();
	return mVolatileInputState;
}

void GHMetroAppLauncher::releaseInputState(void)
{
	if (mInputMutex) mInputMutex->release();
}

GHScreenInfo& GHMetroAppLauncher::getScreenInfo(void)
{
	assert(mRenderServices);
	return mRenderServices->getScreenInfo();
}

GHMessageQueue& GHMetroAppLauncher::getMessageQueue(void)
{
	assert(mAppMessageQueue);
	return *mAppMessageQueue;
}

void GHMetroAppLauncher::pollMouseButtons(void)
{
	pollButtonState(Windows::System::VirtualKey::LeftButton, GHKeyDef::KEY_MOUSE1);
	pollButtonState(Windows::System::VirtualKey::RightButton, GHKeyDef::KEY_MOUSE2);
	pollButtonState(Windows::System::VirtualKey::MiddleButton, GHKeyDef::KEY_MOUSE3);
}

void GHMetroAppLauncher::pollButtonState(Windows::System::VirtualKey windowsKey, GHKeyDef::Enum ghKey)
{
	Windows::UI::Core::CoreVirtualKeyStates keyState = Windows::UI::Core::CoreWindow::GetForCurrentThread()->GetAsyncKeyState(windowsKey);
	bool isPressed = ((keyState & Windows::UI::Core::CoreVirtualKeyStates::Down) == Windows::UI::Core::CoreVirtualKeyStates::Down );
	bool wasPressed = mVolatileInputState.getKeyState(0, ghKey) > 0;

	//I don't trust bool operator==. It has failed me before
	if (isPressed && !wasPressed) {
		mVolatileInputState.handleKeyChange(0, ghKey, 1);
	}
	else if (!isPressed && wasPressed)
	{
		mVolatileInputState.handleKeyChange(0, ghKey, 0);
	}
}

void GHMetroAppLauncher::pollAccelerometer(Windows::Devices::Sensors::Accelerometer^ accelerometer)
{
	if (accelerometer == nullptr) return;

	Windows::Devices::Sensors::AccelerometerReading^ reading = accelerometer->GetCurrentReading();
	if (reading) {
		GHPoint3 accel((float)reading->AccelerationX, (float)reading->AccelerationY, (float)reading->AccelerationZ);
#ifndef WINDOWS8_0
		Windows::Graphics::Display::DisplayInformation^ display = Windows::Graphics::Display::DisplayInformation::GetForCurrentView();		
		if (display->CurrentOrientation == Windows::Graphics::Display::DisplayOrientations::LandscapeFlipped)
		{
			accel[0] *= -1.0f;
			accel[1] *= -1.0f;
		}
		else if (display->CurrentOrientation == Windows::Graphics::Display::DisplayOrientations::Portrait)
		{
			float up = accel[0] * -1.0f;
			float right = accel[1];
			accel[0] = right;
			accel[1] = up;
		}
		else if (display->CurrentOrientation == Windows::Graphics::Display::DisplayOrientations::PortraitFlipped)
		{
			float up = accel[0];
			float right = accel[1] * -1.0f;
			accel[0] = right;
			accel[1] = up;
		}
#endif
		mVolatileInputState.handleAccelerometer(0, accel);
	}
}

void GHMetroAppLauncher::handleWindowSizeChanged(const GHPoint2i& newSize)
{
	GHDebugMessage::outputString("handleWindowSizeChanged %d %d", newSize[0], newSize[1]);

	if (mAppRunner) mAppRunner->getMutex().acquire();

	mRenderServices->getScreenInfo().setSize(newSize);

	GHRenderDeviceDX11* device = (GHRenderDeviceDX11*)mRenderServices->getDevice();
	device->reinit();

	GHMessage resizeMessage(GHMessageTypes::WINDOWRESIZE);
	//mAppMessageQueue->handleMessage(resizeMessage); // test hack
	mSystemServices->getEventMgr().handleMessage(resizeMessage);

	if (mAppRunner) mAppRunner->getMutex().release();
}

GHMetroAppLauncher::DeviceListener::DeviceListener(GHMessageHandler& messageHandler)
	:mMessageHandler(messageHandler)
{
	Windows::Media::Devices::MediaDevice::DefaultAudioRenderDeviceChanged += ref new Windows::Foundation::TypedEventHandler<Platform::Object ^, Windows::Media::Devices::DefaultAudioRenderDeviceChangedEventArgs ^>(this, &GHMetroAppLauncher::DeviceListener::OnDefaultAudioRenderDeviceChanged);
}

void GHMetroAppLauncher::DeviceListener::OnDefaultAudioRenderDeviceChanged(Platform::Object ^sender, Windows::Media::Devices::DefaultAudioRenderDeviceChangedEventArgs ^args)
{
	mMessageHandler.handleMessage(GHMessage(GHBaseIdentifiers::M_NEWAUDIODEVICEAVAILABLE));
}
