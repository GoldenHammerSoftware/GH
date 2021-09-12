// Copyright Golden Hammer Software
#pragma once

#include "GHPlatform/win32/GHWin32Include.h"
#include "GHMath/GHPoint.h"
#include "Base/GHWindow.h"

#define MAX_WINDOWNAME_STRING 100

class GHMessageHandler;

// wrapper for a win32 window
class GHWin32Window : public GHWindow
{
public:
	GHWin32Window(HINSTANCE hInstance, HINSTANCE hPrevInstance,
		int nCmdShow, WNDPROC handleWMFunc,
		const char* windowName, int iconId,
		unsigned int windowWidth = 800, unsigned int windowHeight = 600,
		unsigned int windowX = 0, unsigned int windowY = 0);
	~GHWin32Window(void);

	void setMessageQueue(GHMessageHandler* handler) { mAppMessageQueue = handler; }

	// access to the HWND we wrap.
	HWND getHWND(void) const { return mHWND; }
	// set the HWND that we point to.
	void setHWND(HWND wnd) { mHWND = wnd; }
	WNDCLASSEX getWindowClass(void) const { return mWindowClass; }

	// display the window to the screen.
	void show(void);
	// hide the window, make it invisible.
	void hide(void);
	// destroy the window and cleanup.
	void destroy(void);

	// get the position of the upper left corner of the window.
	const GHPoint2i& getPosition(void);
	// move the window to a given position.
	void setPosition(const GHPoint2i& pos);

	// get the size of the window in pixels.
	const GHPoint2i& getClientAreaSize(void);
	const GHPoint2i& getWindowAreaSize(void);
	const GHPoint2i& getLaunchSize(void) { return mLaunchSize; }

	// push our position and size values to win32 to update the displayed window.
	void updateWindowPosition(void);

	// get the height of the title bar, if a title bar exists.  0 otherwise.
	float getTitleBarSizeInPixels(void);

	// switch between a regular window and a fake fullscreen window.
	// fake fullscreen means a full size window with no title bar.
	void toggleFullscreen(void);
	void enableFullscreen(void);
	void disableFullscreen(void);
	bool isFullscreen(void) const { return mFakeFullscreen; }

	virtual void setFullscreen(bool isFullscreen);
	virtual void resize(int width, int height);

protected:
	HWND mHWND;

	// our main window class.
	WNDCLASSEX mWindowClass;
	// The title bar text
	TCHAR szTitle[MAX_WINDOWNAME_STRING];
	// the main window class name
	TCHAR szWindowClass[MAX_WINDOWNAME_STRING];

	// size and position of the window.
	RECT mWindowRect;
	GHPoint2i mPosition;
	GHPoint2i mClientAreaSize;
	GHPoint2i mWindowAreaSize;
	// creation size
	GHPoint2i mLaunchSize;

	// flag to say whether we are in windows/realfull mode or fake fullscreen mode.
	bool mFakeFullscreen;
	// last known position
	WINDOWPLACEMENT mLastWindowPlacement;
	// the last known size of the non-fake-fullscreen mode.

	GHMessageHandler* mAppMessageQueue;
};
