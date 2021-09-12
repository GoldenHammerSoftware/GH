// Copyright 2010 Golden Hammer Software
#include "GHWPAppLauncher.h"
#include "GHApp.h"
#include "GHMessageQueue.h"
#include "GHRenderDeviceFactoryDX11Managed.h"
#include "GHWPRenderServices.h"
#include "GHGameServices.h"
#include "GHDebugMessage.h"
#include "GHWPSystemServices.h"
#include "GHRenderDeviceDX11Managed.h"
#include "GHAppRunner.h"
#include "GHThreadFactory.h"
#include "GHMutex.h"
#include "GHWPEventInterface.h"

GHWPAppLauncher::GHWPAppLauncher(void)
: mApp(0)
, mAppMessageQueue(0)
, mSystemServices(0)
, mDeviceFactory(0)
, mRenderServices(0)
, mGameServices(0)
, mVolatileInputState(GHWPSystemServices::sMaxPointers, 1, 1, 0, 0)
, mInputMutex(0)
, mAppRunner(0)
, mEventInterface(nullptr)
{
	GHDebugMessage::init(mOutputPipe);
	mSystemServices = new GHWPSystemServices(mSystemControllerMgr);
	mAppMessageQueue = new GHMessageQueue(mSystemServices->mThreadFactory);
	mEventInterface = ref new PhoneDirect3DXamlAppComponent::GHWPEventInterface();
}

GHWPAppLauncher::~GHWPAppLauncher(void)
{
	delete mAppRunner;
	delete mApp;
	delete mInputMutex;
	delete mGameServices;
	delete mRenderServices;
	delete mSystemServices;
	delete mAppMessageQueue;
}

void GHWPAppLauncher::init(ID3D11Device1* device, ID3D11DeviceContext1* context, 
						   ID3D11RenderTargetView* renderTargetView,
						   int screenWidth, int screenHeight)
{
	GHPoint2i screenSize(screenWidth, screenHeight);

	mDeviceFactory = new GHRenderDeviceFactoryDX11Managed(device, context, renderTargetView, mSystemServices->mEventMgr);
	mRenderServices = new GHWPRenderServices(*mSystemServices, mDeviceFactory, screenSize);
	mGameServices = new GHGameServices(*mSystemServices, *mRenderServices, *mAppMessageQueue);

	initApp();

	mInputMutex = mSystemServices->mThreadFactory->createMutex();
	mAppRunner = new GHAppRunner(mApp, mSystemServices->mEventMgr, *mAppMessageQueue, *mSystemServices->mInputState, 
		mVolatileInputState, *mInputMutex, *mSystemServices->mThreadFactory, 0);
}

void GHWPAppLauncher::updateD3D(ID3D11Device1* device, ID3D11DeviceContext1* context, 
								ID3D11RenderTargetView* renderTargetView)
{
	if (!mApp) return;
	((GHRenderDeviceDX11Managed*)mRenderServices->getDevice())->updateSurfaces(device, context, renderTargetView);
}

void GHWPAppLauncher::runFrame(void)
{
	if (!mApp) return;

	//long long mem = Windows::Phone::System::Memory::MemoryManager::ProcessCommittedBytes;
	//GHDebugMessage::outputString("Mem %llu", mem);

	mAppRunner->runFrame();
	mSystemControllerMgr.update();
}

GHInputState& GHWPAppLauncher::beginInput(void)
{
	mInputMutex->acquire();
	return mVolatileInputState;
}

void GHWPAppLauncher::endInput(void)
{
	if (!mInputMutex) return;
	mInputMutex->release();
}

bool GHWPAppLauncher::checkExitOnBack(void) const
{
	if (!mSystemServices) return false;
	GHWPSystemServices* wpServices = (GHWPSystemServices*)mSystemServices;
	return wpServices->checkExitOnBack();
}

void GHWPAppLauncher::handleInterrupt(bool active)
{
	if (mAppRunner) {
		mAppRunner->handleInterrupt(active);
	}
}
