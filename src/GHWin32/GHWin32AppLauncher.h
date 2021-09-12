// Copyright Golden Hammer Software
#pragma once

#include "GHPlatform/win32/GHWin32Include.h"
#include "GHUtils/GHControllerMgr.h"
#include "GHMath/GHPoint.h"

class GHApp;
class GHSystemServices;
class GHRenderServices;
class GHGameServices;
class GHMessageQueue;
class GHWin32LaunchCfg;
class GHWin32Window;
class GHInputState;

// handles launching a ghapp for win32
class GHWin32AppLauncher
{
public:
	GHWin32AppLauncher(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPTSTR lpCmdLine, int nCmdShow,
		GHWin32LaunchCfg& cfg);
	~GHWin32AppLauncher(void);

	void run(GHApp& app);

	// handle a windows message and send the info to the active user input interfaces.
	LRESULT CALLBACK handleWindowsMessage(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

	GHSystemServices& getSystemServices(void) { return *mSystemServices; }
	GHRenderServices& getRenderServices(void) { return *mRenderServices; }
	GHGameServices& getGameServices(void) { return *mGameServices; }
	GHMessageQueue& getMessageQueue(void) { return *mAppMessageQueue; }
	GHControllerMgr& getControllerMgr(void) { return mSystemControllerMgr; }
	GHWin32Window* getWindow(void) { return mWindow; }

private:
	// handle key/mouse input commands.
	// return false if it wasn't one that we processed.
	bool handleInputMessage(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
	// do code for a mouse move windows message
	bool handleMouseMove(GHPoint2& mousePos, GHInputState& inputState);
	// store out that a window resize happened and we will need to recreate the renderer.
	void handleWindowResize(int width, int height);
	// send appropriate messages in response to the window resizing.
	void updateWindowSize(void);
	//register for notifications when devices (eg headphones) are plugged in or lost
	bool registerForDeviceNotifications(HWND hWnd);
	// return true if there were any touch inputs handled.
	bool handleTouchInput(WPARAM wParam, LPARAM lParam);
	int findTouchIndex(DWORD touchID);
	bool detectMouse(GHInputState& inputState);
	void calcLaunchWindowSize(int& windowWidth, int& windowHeight);

private:
	bool mInitialized;
	int mMouseDetected; //we get a WM_MOUSEMOVE even if there isn't a mouse. We use this to ignore the first one so that buttons don't get stuck.
	GHApp* mApp;
	GHSystemServices* mSystemServices;
	GHRenderServices* mRenderServices;
	GHGameServices* mGameServices;
	GHMessageQueue* mAppMessageQueue;
	GHWin32Window* mWindow;
	GHControllerMgr mSystemControllerMgr;

	bool mWasMouseCaptured;
	bool mIgnoreNextMouseMove;
	GHPoint2 mLastMousePos;
	// the position where we reset the cursor to. Same as mLastMousePos but in monitor space
	GHPoint2i mMouseCapturePos;

	// a flag to say whether we let windows handle real fullscreen, or if we fake it with HWND modification.
	bool mAllowDXFullscreen;

	// a counter we use to say a window change happened and we should notify stuff eventually.
	int mWindowUpdateCount;

	const static int sMaxTouches = 10;
	DWORD mTouches[sMaxTouches];

	//handle for device notifications
	HDEVNOTIFY mHDevNotify;
};
