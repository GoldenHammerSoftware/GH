// Copyright 2010 Golden Hammer Software
#pragma once

#include "GHMetroOutputPipe.h"
#include <d3d11_1.h>
#include "GHInputState.h"
#include "GHControllerMgr.h"
#include "GHWPEventInterface.h"

class GHApp;
class GHMessageQueue;
class GHSystemServices;
class GHRenderDeviceFactoryDX11;
class GHRenderServices;
class GHGameServices;
class GHAppRunner;
class GHMutex;

// wrapper for common things needed by every wp8 app.
class GHWPAppLauncher
{
public:
	GHWPAppLauncher(void);
	virtual ~GHWPAppLauncher(void);

	// create all our services and then call init app.
	void init(ID3D11Device1* device, ID3D11DeviceContext1* context, ID3D11RenderTargetView* renderTargetView,
		int screenWidth, int screenHeight);
	void updateD3D(ID3D11Device1* device, ID3D11DeviceContext1* context, ID3D11RenderTargetView* renderTargetView);
	void runFrame(void);

	// create the individual app, such as bowling app.
	virtual void initApp(void) = 0;

	GHInputState& beginInput(void);
	void endInput(void);

	bool checkExitOnBack(void) const;

	PhoneDirect3DXamlAppComponent::GHWPEventInterface^ getEventInterface(void) { return mEventInterface; }
	GHMessageQueue* getAppMessageQueue(void) { return mAppMessageQueue; }
	// forward to mAppRunner
	void handleInterrupt(bool active);

protected:
	GHApp* mApp;
	GHMetroOutputPipe mOutputPipe;  
	GHMessageQueue* mAppMessageQueue;
    GHSystemServices* mSystemServices;
	GHRenderDeviceFactoryDX11* mDeviceFactory;
    GHRenderServices* mRenderServices;
    GHGameServices* mGameServices;
	GHControllerMgr mSystemControllerMgr;
	PhoneDirect3DXamlAppComponent::GHWPEventInterface^ mEventInterface;

	GHMutex* mInputMutex;
	GHInputState mVolatileInputState;
	GHAppRunner* mAppRunner;
};
