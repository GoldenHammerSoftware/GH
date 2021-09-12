// Copyright Golden Hammer Software
#pragma once

#include "GHWin32Window.h"
#include "GHPlatform/GHDebugMessage.h"
#include <stdio.h>
#include "GHPlatform/win32/GHLChar.h"
#include "GHUtils/GHMessageHandler.h"
#include "GHUtils/GHMessage.h"
#include "GHUtils/GHUtilsIdentifiers.h"
#include "GHBaseIdentifiers.h" 

GHWin32Window::GHWin32Window(HINSTANCE hInstance, HINSTANCE hPrevInstance,
	int nCmdShow, WNDPROC handleWMFunc,
	const char* windowName, int iconId,
	unsigned int windowWidth, unsigned int windowHeight,
	unsigned int windowX, unsigned int windowY)
: mHWND(0)
, mFakeFullscreen(false)
, mAppMessageQueue(0)
{
	wchar_t* wideName = GHLChar::convertToWide(windowName);

	// register the window class.
	mWindowClass.cbSize = sizeof(WNDCLASSEX);
	mWindowClass.style = CS_HREDRAW | CS_VREDRAW;
	mWindowClass.lpfnWndProc = handleWMFunc;
	mWindowClass.cbClsExtra = 0;
	mWindowClass.cbWndExtra = 0;
	mWindowClass.hInstance = hInstance;
	//mWindowClass.hIcon = 0;//LoadIcon(hInstance, "bfp2app.ico");
	mWindowClass.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(iconId));
	mWindowClass.hCursor = LoadCursor(NULL, IDC_ARROW);
	mWindowClass.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
	mWindowClass.lpszMenuName = wideName;
	mWindowClass.lpszClassName = wideName;
	mWindowClass.hIconSm = LoadIcon(mWindowClass.hInstance, L"small.ico");
	RegisterClassEx(&mWindowClass);

	// create the window.
	mHWND = CreateWindow(
		wideName,
		wideName,
		WS_CAPTION | WS_SYSMENU | WS_MAXIMIZEBOX | WS_MINIMIZEBOX | WS_SIZEBOX,
		CW_USEDEFAULT,
		0,
		windowWidth, windowHeight,
		NULL,
		NULL,
		hInstance,
		NULL);

	delete wideName;

	if (!mHWND) {
		GHDebugMessage::outputString("Failed to create window!\n");
		return;
	}
}

GHWin32Window::~GHWin32Window(void)
{
}

void GHWin32Window::show(void)
{
	if (!mHWND) return;
	ShowWindow(mHWND, SW_SHOW);
	UpdateWindow(mHWND);
}

void GHWin32Window::hide(void)
{
	if (!mHWND) return;
	ShowWindow(mHWND, SW_HIDE);
}

void GHWin32Window::destroy(void)
{
	if (!mHWND) return;
	DestroyWindow(mHWND);
}

const GHPoint2i& GHWin32Window::getPosition(void)
{
	if (mHWND) {
		// ask windows where our window really is.
		GetWindowRect(mHWND, &mWindowRect);
	}
	mPosition[0] = mWindowRect.left;
	mPosition[1] = mWindowRect.top;
	return mPosition;
}

void GHWin32Window::setPosition(const GHPoint2i& pos)
{
	const GHPoint2i& windowSize = getClientAreaSize();
	mWindowRect.left = pos[0];
	mWindowRect.top = pos[1];
	mWindowRect.right = mWindowRect.left + windowSize[0];
	mWindowRect.bottom = mWindowRect.top + windowSize[1];
	updateWindowPosition();
}

void GHWin32Window::resize(int width, int height)
{
	mWindowRect.right = mWindowRect.left + width;
	mWindowRect.bottom = mWindowRect.top + height;
	updateWindowPosition();
}

const GHPoint2i& GHWin32Window::getClientAreaSize(void)
{
	if (mHWND) {
		// ask windows where our window really is.
		GetClientRect(mHWND, &mWindowRect);
	}
	mClientAreaSize[0] = mWindowRect.right - mWindowRect.left;
	mClientAreaSize[1] = mWindowRect.bottom - mWindowRect.top;
	return mClientAreaSize;
}

const GHPoint2i& GHWin32Window::getWindowAreaSize(void)
{
	RECT rect;
	if (mHWND) {
		// ask windows where our window really is.
		GetWindowRect(mHWND, &rect);
	}
	mWindowAreaSize[0] = rect.right - rect.left;
	mWindowAreaSize[1] = rect.bottom - rect.top;
	return mWindowAreaSize;
}

void GHWin32Window::updateWindowPosition(void)
{
	if (!mHWND) return;

	// Ok win32 is dumb here.
	// Our code is dealing in client area size.  SetWindowPos deals in full window size.
	// We need to calculate how big the padding is for the window and add that to our intended val.
	RECT windowRect, clientRect;
	GetWindowRect(mHWND, &windowRect);
	GetClientRect(mHWND, &clientRect);
	int xExtra = windowRect.right - windowRect.left - clientRect.right;
	int yExtra = windowRect.bottom - windowRect.top - clientRect.bottom;

	SetWindowPos(mHWND, HWND_TOP,
		mWindowRect.left, mWindowRect.top,
		(mWindowRect.right - mWindowRect.left) + xExtra,
		(mWindowRect.bottom - mWindowRect.top) + yExtra,
		0);

	char buff[200];
	::sprintf(buff, "New WinPos: %d, %d, %d, %d\n",
		mWindowRect.left, mWindowRect.right,
		mWindowRect.top, mWindowRect.bottom);
	GHDebugMessage::outputString(buff);

	mClientAreaSize[0] = mWindowRect.right - mWindowRect.left;
	mClientAreaSize[1] = mWindowRect.bottom - mWindowRect.top;
	mPosition[0] = mWindowRect.left;
	mPosition[1] = mWindowRect.top;
}

float GHWin32Window::getTitleBarSizeInPixels(void)
{
	TITLEBARINFO pti;
	pti.cbSize = sizeof(TITLEBARINFO);
	if (GetTitleBarInfo(mHWND, &pti)) {
		return (float)(pti.rcTitleBar.bottom - pti.rcTitleBar.top);
	}
	else {
		DWORD lastErr = GetLastError();
		int brkpt = 1;
	}
	return 0;
}

void GHWin32Window::toggleFullscreen(void)
{
	// http://blogs.msdn.com/b/oldnewthing/archive/2010/04/12/9994016.aspx

	if (!mFakeFullscreen)
	{
		enableFullscreen();
	}
	else
	{
		disableFullscreen();
	}
}

void GHWin32Window::setFullscreen(bool isFullscreen)
{
	if (isFullscreen)
	{
		enableFullscreen();
	}
	else
	{
		disableFullscreen();
	}
}

void GHWin32Window::enableFullscreen(void)
{
	MONITORINFO mi = { sizeof(mi) };
	if (GetWindowPlacement(mHWND, &mLastWindowPlacement) &&
		GetMonitorInfo(MonitorFromWindow(mHWND,
			MONITOR_DEFAULTTOPRIMARY), &mi)) {

		LONG style = WS_OVERLAPPED;
		SetWindowLongPtr(mHWND, GWL_STYLE, style);

		WINDOWPLACEMENT newPlacement;
		newPlacement.length = sizeof(WINDOWPLACEMENT);
		newPlacement.flags = 0;
		newPlacement.showCmd = SW_SHOW;
		newPlacement.rcNormalPosition = mi.rcMonitor;
		SetWindowPlacement(mHWND, &newPlacement);

		SetWindowPos(mHWND, NULL, 0, 0, 0, 0,
			SWP_NOMOVE | SWP_NOSIZE | SWP_NOZORDER |
			SWP_NOOWNERZORDER | SWP_FRAMECHANGED);
	}

	mFakeFullscreen = true;

	if (mAppMessageQueue)
	{
		GHMessage mes(GHBaseIdentifiers::M_FULLSCREENENABLED);
		mAppMessageQueue->handleMessage(mes);
	}
}

void GHWin32Window::disableFullscreen(void)
{
	LONG style = WS_CAPTION | WS_SYSMENU | WS_MAXIMIZEBOX | WS_MINIMIZEBOX | WS_SIZEBOX;
	SetWindowLongPtr(mHWND, GWL_STYLE, style);

	SetWindowPlacement(mHWND, &mLastWindowPlacement);
	SetWindowPos(mHWND, NULL, 0, 0, 0, 0,
		SWP_NOMOVE | SWP_NOSIZE | SWP_NOZORDER |
		SWP_NOOWNERZORDER | SWP_FRAMECHANGED);

	mFakeFullscreen = false;

	if (mAppMessageQueue)
	{
		GHMessage mes(GHBaseIdentifiers::M_FULLSCREENDISABLED);
		mAppMessageQueue->handleMessage(mes);
	}
}
