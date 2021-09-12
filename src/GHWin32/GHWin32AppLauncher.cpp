// Copyright Golden Hammer Software
#include "GHWin32AppLauncher.h"
#include "GHWin32SystemServices.h"
#include "GHPlatform/GHDebugMessage.h"
#include "GHApp.h"
#include "GHWin32Window.h"
#include "GHGameServices.h"
#include "GHUtils/GHMessageQueue.h"
#include "GHWin32LaunchCfg.h"
#include "GHPlatform/GHFileOpener.h"
#include "GHRenderDeviceFactoryDX11Native.h"
#include "GHWin32SwapChainCreator.h"
#include "GHRenderDeviceFactoryDX11Native.h"
#include "GHWin32DX11RenderServices.h"
#include "GHInputState.h"
#include "GHKeyDef.h"
#include "GHXInputHandler.h"
#include "GHRenderDeviceDX11.h"
#include "GHMessageTypes.h"
#include "GHUtils/GHMessage.h"
#include "GHWin32MouseCapturer.h"
#include <Dbt.h>
#include <Ks.h>
#include <Ksmedia.h>
#include "GHBaseIdentifiers.h"

// debug setting to allow arbitrary window sizes
#define FREEFORM_WINDOW_SIZE
// debug setting to launch in portrait mode.
//#define LAUNCH_IN_PORTRAIT

// some statics to connect to the windows api.
static GHWin32AppLauncher* sAppLauncher = 0;
LRESULT CALLBACK	WndProc(HWND, UINT, WPARAM, LPARAM);

GHWin32AppLauncher::GHWin32AppLauncher(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPTSTR lpCmdLine, int nCmdShow,
	GHWin32LaunchCfg& cfg)
: mWindow(0)
, mInitialized(false)
, mWindowUpdateCount(0)
, mWasMouseCaptured(false)
, mIgnoreNextMouseMove(false)
, mMouseDetected(0)
, mAllowDXFullscreen(cfg.mAllowFullscreen)
{
	CoInitializeEx(nullptr, COINIT_APARTMENTTHREADED);

	// debug code.
	TCHAR NPath[MAX_PATH];
	GetCurrentDirectory(MAX_PATH, NPath);

	for (int i = 0; i < sMaxTouches; ++i)
	{
		mTouches[i] = 0;
	}

	sAppLauncher = this;

	int windowWidth, windowHeight;
	calcLaunchWindowSize(windowWidth, windowHeight);
	mWindow = new GHWin32Window(hInstance, hPrevInstance,
		nCmdShow, WndProc, cfg.mWindowName, cfg.mIconId,
		windowWidth, windowHeight, 0, 0);

	mSystemServices = new GHWin32SystemServices(*mWindow, mSystemControllerMgr);
	for (size_t i = 0; i < cfg.mFileDirs.size(); ++i)
	{
		mSystemServices->getPlatformServices().getFileOpener().addResourcePath(cfg.mFileDirs[i]);
	}
	mAppMessageQueue = new GHMessageQueue(&mSystemServices->getPlatformServices().getThreadFactory());

	mWindow->setMessageQueue(mAppMessageQueue);

	GHWin32SwapChainCreator* swapChainCreator = new GHWin32SwapChainCreator(*mWindow, mAllowDXFullscreen);
	GHRenderDeviceFactoryDX11Native* rdFactory = new GHRenderDeviceFactoryDX11Native(*swapChainCreator, *mAppMessageQueue, 4);

	mRenderServices = new GHWin32DX11RenderServices(*mSystemServices, rdFactory, swapChainCreator, mWindow->getClientAreaSize());

	mGameServices = new GHGameServices(*mSystemServices, *mRenderServices, *mAppMessageQueue);

	//win32 api call -- make WM_TOUCH messages appear
	RegisterTouchWindow(mWindow->getHWND(), 0);
	registerForDeviceNotifications(mWindow->getHWND());

	mWindow->show();
	mInitialized = true;
}

GHWin32AppLauncher::~GHWin32AppLauncher(void)
{
	delete mAppMessageQueue;
	delete mGameServices;
	delete mRenderServices;
	delete mSystemServices;
	//delete mWindow; // deleted in system services.
}

void GHWin32AppLauncher::run(GHApp& app)
{
	mApp = &app;

	MSG msg;
	GHDebugMessage::outputString("**** Starting main loop.\n");

	// Main message loop:
	bool running = true;
	while (running) 
	{
		// process system messages.
		while (::PeekMessage(&msg, 0, 0, 0, PM_REMOVE)) {
			TranslateMessage(&msg);
			DispatchMessage(&msg);

			if (msg.message == WM_QUIT) {
				running = false;
				break;
			}
		}

		updateWindowSize();
		GHXInputHandler::pollGamepads(mSystemServices->getInputState());
		mAppMessageQueue->sendMessages(mSystemServices->getEventMgr());
		mApp->runFrame();
		mSystemServices->getInputState().clearInputEvents();
		mSystemControllerMgr.update();
	}

	GHDebugMessage::outputString("**** Ending main loop.\n");
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	if (!sAppLauncher) {
		return DefWindowProc(hWnd, message, wParam, lParam);
	}
	return sAppLauncher->handleWindowsMessage(hWnd, message, wParam, lParam);
}

static GHKeyDef::Enum convertWindowsToGHKey(unsigned char key)
{
	if (key == VK_LEFT) {
		return GHKeyDef::KEY_LEFTARROW;
	}
	if (key == VK_RIGHT) {
		return GHKeyDef::KEY_RIGHTARROW;
	}
	if (key == VK_UP) {
		return GHKeyDef::KEY_UPARROW;
	}
	if (key == VK_DOWN) {
		return GHKeyDef::KEY_DOWNARROW;
	}
	if (key == VK_SHIFT) {
		return GHKeyDef::KEY_SHIFT;
	}
	if (key == VK_CONTROL) {
		return GHKeyDef::KEY_CONTROL;
	}
	if (key == VK_ESCAPE) {
		return GHKeyDef::KEY_ESC;
	}
	if (key == VK_RETURN) {
		return GHKeyDef::KEY_ENTER;
	}

	if (key == 219) {
		return (GHKeyDef::Enum)'[';
	}
	if (key == 220) {
		return (GHKeyDef::Enum)'\\';
	}
	if (key == 221) {
		return (GHKeyDef::Enum)']';
	}
	if (key == 186) {
		return (GHKeyDef::Enum)';';
	}
	if (key == 222) {
		return (GHKeyDef::Enum)'\'';
	}
	if (key == 188) {
		return (GHKeyDef::Enum)',';
	}
	if (key == 190) {
		return (GHKeyDef::Enum)'.';
	}
	if (key == 191) {
		return (GHKeyDef::Enum)'/';
	}
	if (key == 192) {
		return (GHKeyDef::Enum)'`';
	}
	if (key == 46) {
		return GHKeyDef::KEY_DELETE;
	}
	if (key == 187) {
		return (GHKeyDef::Enum)'=';
	}
	if (key == 189) {
		return (GHKeyDef::Enum)'-';
	}
	if (key == '\b') {
		return GHKeyDef::KEY_BACKSPACE;
	}

	return (GHKeyDef::Enum)::tolower(key);
}

bool GHWin32AppLauncher::handleInputMessage(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	bool handled = false;
	unsigned char key;
	GHKeyDef::Enum ghKey;
	GHPoint2 mousePos;
	GHInputState& inputState = sAppLauncher->getSystemServices().getInputState();
	const GHScreenInfo& screenInfo = sAppLauncher->getRenderServices().getScreenInfo();

	switch (message) {
	case WM_KEYDOWN:
		key = (char)wParam;
		//GHDebugMessage::outputString("Key %c %d", key, key);
		ghKey = convertWindowsToGHKey(key);
		inputState.handleKeyChange(0, ghKey, true);
		handled = true;
		break;
	case WM_KEYUP:
		key = (char)wParam;
		ghKey = convertWindowsToGHKey(key);
		inputState.handleKeyChange(0, ghKey, false);
		handled = true;
		break;
	case WM_CHAR:
		//key = (char)wParam;
		//handled = true;
		break;
	case WM_SYSKEYDOWN:
		if (!mAllowDXFullscreen)
		{
			key = (char)wParam;
			if (key == VK_RETURN)
			{
				if (GetAsyncKeyState(VK_MENU))
				{
					sAppLauncher->getWindow()->toggleFullscreen();
					handled = true;
				}
			}
		}
		break;
		/*
		case WM_SYSKEYDOWN:
		key = (char)wParam;
		ghKey = convertWindowsToGHKey(key);
		if (key != -1) {
		inputState->handleKeyChange(0, ghKey, true);
		handled = true;
		}
		break;
		case WM_SYSKEYUP:
		key = (char)wParam;
		ghKey = convertWindowsToGHKey(key);
		if (key != -1) {
		inputState->handleKeyChange(0, ghKey, false);
		handled = true;
		}
		break;
		*/
	case WM_TOUCH:
	{
		handled |= handleTouchInput(wParam, lParam);
	}
		break;
	case WM_MOUSEMOVE:
		if (!detectMouse(inputState))
		{
			break;
		}

		if (GetMessageExtraInfo() == 0)
		{
			// this code gets stuff relative to the client area.
			mousePos.setCoords(LOWORD(lParam), HIWORD(lParam));
			mousePos[0] /= (float)screenInfo.getSize()[0];
			mousePos[1] /= (float)screenInfo.getSize()[1];
			handled |= handleMouseMove(mousePos, inputState);
		}
		break;
	case WM_LBUTTONDOWN:
		if (GetMessageExtraInfo() == 0)
		{
			inputState.handleKeyChange(0, GHKeyDef::KEY_MOUSE1, 1);
			handled = true;
		}
		break;
	case WM_LBUTTONUP:
		if (GetMessageExtraInfo() == 0)
		{
			inputState.handleKeyChange(0, GHKeyDef::KEY_MOUSE1, 0);
			handled = true;
		}
		break;
	case WM_MBUTTONDOWN:
		inputState.handleKeyChange(0, GHKeyDef::KEY_MOUSE3, 1);
		handled = true;
		break;
	case WM_MBUTTONUP:
		inputState.handleKeyChange(0, GHKeyDef::KEY_MOUSE3, 0);
		handled = true;
		break;
	case WM_RBUTTONDOWN:
		inputState.handleKeyChange(0, GHKeyDef::KEY_MOUSE2, 1);
		handled = true;
		break;
	case WM_RBUTTONUP:
		inputState.handleKeyChange(0, GHKeyDef::KEY_MOUSE2, 0);
		handled = true;
		break;
	case WM_MOUSELEAVE:
		GHDebugMessage::outputString("WM_MOUSELEAVE");
		handled = true;
		break;
	case WM_MOUSEWHEEL:
	{
		short wheelDeltaShort = GET_WHEEL_DELTA_WPARAM(wParam);
		// mouse wheel input comes in as multiples of 120.
		float wheelFloat = wheelDeltaShort / 120.0f;
		// you can never look at the state of mouse wheel on the input state, have to checkKeyChange.
		inputState.handleKeyChange(0, GHKeyDef::KEY_MOUSEWHEEL, wheelFloat);
		inputState.handleKeyChange(0, GHKeyDef::KEY_MOUSEWHEEL, 0);
		handled = true;
		break;
	}
	}

	return handled;
}

bool GHWin32AppLauncher::handleTouchInput(WPARAM wParam, LPARAM lParam)
{
	bool handled = false;
	const GHScreenInfo& screenInfo = sAppLauncher->getRenderServices().getScreenInfo();
	GHInputState& inputState = sAppLauncher->getSystemServices().getInputState();

	UINT numInputs = LOWORD(wParam);
	static std::vector<TOUCHINPUT> touchInputArray;
	touchInputArray.resize(numInputs);
	if (GetTouchInputInfo((HTOUCHINPUT)lParam, numInputs, &touchInputArray.front(), sizeof(TOUCHINPUT)))
	{
		for (UINT i = 0; i < numInputs; ++i)
		{
			TOUCHINPUT ti = touchInputArray[i];
			int touchIndex = findTouchIndex(ti.dwID);
			if (touchIndex != -1)
			{
				float posX = ((float)ti.x) / 100.f;
				float posY = ((float)ti.y) / 100.f;
				posX -= mWindow->getPosition()[0];
				posY -= mWindow->getPosition()[1];
				posY -= mWindow->getTitleBarSizeInPixels();

				posX /= (float)screenInfo.getSize()[0];
				posY /= (float)screenInfo.getSize()[1];
				if (ti.dwFlags & TOUCHEVENTF_MOVE)
				{
					inputState.handlePointerChange(touchIndex, GHPoint2(posX, posY));
				}
				if (ti.dwFlags & TOUCHEVENTF_DOWN)
				{
					inputState.handlePointerChange(touchIndex, GHPoint2(posX, posY));
					inputState.handlePointerActive(touchIndex, true);
					inputState.handleKeyChange(0, (int)GHKeyDef::KEY_MOUSE1 + (touchIndex * inputState.getNumMouseButtons()), true);
				}
				if (ti.dwFlags & TOUCHEVENTF_UP)
				{
					mTouches[touchIndex] = 0;
					inputState.handleKeyChange(0, (int)GHKeyDef::KEY_MOUSE1 + (touchIndex * inputState.getNumMouseButtons()), false);
					inputState.handlePointerActive(touchIndex, false);
					inputState.handlePointerChange(touchIndex, GHPoint2(-1, -1));
				}
			}
		}
		handled = true;
		CloseTouchInputHandle((HTOUCHINPUT)lParam);
	}

	return handled;
}

bool GHWin32AppLauncher::detectMouse(GHInputState& inputState)
{
	if (mMouseDetected > 1) {
		return true;
	}

	if (mMouseDetected == 1) {
		inputState.handlePointerActive(0, true);
		return true;
	}

	if (mMouseDetected == 0) {
		mMouseDetected = 1;
	}
	return false;

}

bool GHWin32AppLauncher::handleMouseMove(GHPoint2& mousePos, GHInputState& inputState)
{
	// if we call set position and don't abort we will infinite loop.
	if (mIgnoreNextMouseMove) {
		mIgnoreNextMouseMove = false;
		return false;
	}

	bool isCaptured = ((GHWin32MouseCapturer&)getSystemServices().getMouseCapturer()).isCaptured();
	if (isCaptured)
	{
		if (!mWasMouseCaptured) {
			mLastMousePos = mousePos;
			mousePos.setCoords(0, 0);
			
			// this code gets mouse in absolute space.
			POINT currPos;
			if (::GetCursorPos(&currPos)) {
				mMouseCapturePos.setCoords(currPos.x, currPos.y);
			}
		}
		else {
			mousePos -= mLastMousePos;
		}
		mousePos += inputState.getPointerPosition(0);

		mIgnoreNextMouseMove = true;
		SetCursorPos(mMouseCapturePos[0], mMouseCapturePos[1]);
		mWasMouseCaptured = true;
	}
	else
	{
		mWasMouseCaptured = false;
	}

	inputState.handlePointerChange(0, mousePos);
	return true;
}

void GHWin32AppLauncher::handleWindowResize(int width, int height)
{
	// cache the value and set a counter so we don't keep recreating the device.
	mWindowUpdateCount = 30;
}

void GHWin32AppLauncher::updateWindowSize(void)
{
	if (mWindowUpdateCount == 0) return;
	if (mWindowUpdateCount == 1)
	{
		GHPoint2i newSize = mWindow->getClientAreaSize();
		mRenderServices->getScreenInfo().setSize(newSize);

		GHRenderDeviceDX11* device = (GHRenderDeviceDX11*)mRenderServices->getDevice();
		device->reinit();

		GHMessage resizeMessage(GHMessageTypes::WINDOWRESIZE);
		mSystemServices->getEventMgr().handleMessage(resizeMessage);
	}
	mWindowUpdateCount--;
}

LRESULT CALLBACK GHWin32AppLauncher::handleWindowsMessage(
	HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	if (!mInitialized)
	{
		return DefWindowProc(hWnd, message, wParam, lParam);
	}

	//int wmId, wmEvent;
	PAINTSTRUCT ps;
	HDC hdc;
	int width, height;

	if (handleInputMessage(hWnd, message, wParam, lParam)) {
		return 0;
	}

	bool handled = false;
	switch (message) {
	case WM_CLOSE:
		UnregisterDeviceNotification(mHDevNotify);
		mHDevNotify = NULL;
		break;
	case WM_PAINT:
		hdc = BeginPaint(hWnd, &ps);
		EndPaint(hWnd, &ps);
		handled = true;
		break;
	case WM_DESTROY:
		PostQuitMessage(0);
		handled = true;
		break;
	case WM_GETMINMAXINFO:
		//MINMAXINFO* info = (MINMAXINFO*)lParam;
		// if we want to edit window min or max do it here.
		break;
	case WM_NCCREATE:
		//CREATESTRUCT* args = (CREATESTRUCT*)lParam;
		break;
	case WM_COMMAND:
		//wmId = LOWORD(wParam);
		//wmEvent = HIWORD(wParam);
		break;
	case WM_SIZE:
		if (wParam == SIZE_MAXIMIZED || wParam == SIZE_RESTORED)
		{
			// restored is actually resize.
			width = LOWORD(lParam);
			height = HIWORD(lParam);
			handleWindowResize(width, height);
			handled = true;
		}
		else if (wParam == SIZE_MINIMIZED)
		{
			// pause?
		}
		break;
	case WM_DEVICECHANGE:
	{
		//new device available
		if (wParam == DBT_DEVICEARRIVAL)
		{
			PDEV_BROADCAST_DEVICEINTERFACE devInterface = (PDEV_BROADCAST_DEVICEINTERFACE)lParam;
			if (devInterface)
			{
				//new device is an audio device
				if (!memcmp(&devInterface->dbcc_classguid, &KSCATEGORY_AUDIO, sizeof(GUID)))
				{
					GHMessage message(GHBaseIdentifiers::M_NEWAUDIODEVICEAVAILABLE);
					if (mAppMessageQueue) { mAppMessageQueue->handleMessage(message); }
				}
			}
		}
		break;
	}
	case WM_SIZING:
	{
		// maintain the aspect ratio.
		LPRECT outRect = (LPRECT)lParam;

		LONG height = outRect->bottom - outRect->top;
		LONG width = outRect->right - outRect->left;

// defined at the top of this file if in use
#ifdef FREEFORM_WINDOW_SIZE
		LONG adjustedHeight = height;
		LONG adjustedWidth = width;
#else
		const GHPoint2i aspectSize = GHPoint2i(800, 450);
		const float aspectWidth = (float)aspectSize[0];
		const float aspectHeight = (float)aspectSize[1];
		const float desiredWidthRatio = aspectWidth / aspectHeight;
		const float desiredHeightRatio = aspectHeight / aspectWidth;

		LONG adjustedHeight = (LONG)(desiredHeightRatio * (float)width);
		LONG adjustedWidth = (LONG)(desiredWidthRatio * (float)height);
#endif

		if (wParam == WMSZ_LEFT) {
			outRect->left = outRect->right - width;
			outRect->bottom = outRect->top + adjustedHeight;
		}
		else if (wParam == WMSZ_RIGHT) {
			outRect->right = outRect->left + width;
			outRect->bottom = outRect->top + adjustedHeight;
		}
		else if (wParam == WMSZ_TOP) {
			outRect->top = outRect->bottom - height;
			outRect->right = outRect->left + adjustedWidth;
		}
		else if (wParam == WMSZ_BOTTOM) {
			outRect->bottom = outRect->top + height;
			outRect->right = outRect->left + adjustedWidth;
		}
		else if (wParam == WMSZ_BOTTOMRIGHT) {
			outRect->right = outRect->left + width;
			outRect->bottom = outRect->top + adjustedHeight;
		}
		else if (wParam == WMSZ_BOTTOMLEFT)
		{
			outRect->left = outRect->right - width;
			outRect->bottom = outRect->top + adjustedHeight;
		}
		else if (wParam == WMSZ_TOPRIGHT)
		{
			outRect->top = outRect->bottom - height;
			outRect->right = outRect->left + width;
		}
		else if (wParam == WMSZ_TOPLEFT)
		{
			outRect->top = outRect->bottom - height;
			outRect->left = outRect->right - adjustedWidth;
		}
		handled = true;
	}
	}
	if (handled) {
		return 0;
	}
	else {
		return DefWindowProc(hWnd, message, wParam, lParam);
	}
}

int GHWin32AppLauncher::findTouchIndex(DWORD touchID) 
{
	// first look for an existing match.
	for (int i = 1; i < sMaxTouches; ++i) {
		if (mTouches[i] && touchID)
		{
			DWORD currTouch = mTouches[i];
			if (currTouch == touchID)
			{
				return i;
			}
		}
	}
	// then look for an open slot.
	for (int i = 1; i < sMaxTouches; ++i) {
		if (mTouches[i] == 0) {
			mTouches[i] = touchID;
			return i;
		}
	}
	return -1;
}

bool GHWin32AppLauncher::registerForDeviceNotifications(HWND hWnd)
{
	DEV_BROADCAST_DEVICEINTERFACE notificationFilter;
	ZeroMemory(&notificationFilter, sizeof(notificationFilter));
	notificationFilter.dbcc_size = sizeof(DEV_BROADCAST_DEVICEINTERFACE);
	notificationFilter.dbcc_devicetype = DBT_DEVTYP_DEVICEINTERFACE;
	notificationFilter.dbcc_classguid = KSCATEGORY_AUDIO;

	mHDevNotify = RegisterDeviceNotification(hWnd, &notificationFilter, DEVICE_NOTIFY_WINDOW_HANDLE);
	return mHDevNotify != NULL;
}

void GHWin32AppLauncher::calcLaunchWindowSize(int& windowWidth, int& windowHeight)
{
#ifdef LAUNCH_IN_PORTRAIT
	int screenHeight = GetSystemMetrics(SM_CYSCREEN);
	float widthRatio = 450.0f / 800.0f;

	windowHeight = screenHeight * 4 / 5;
	windowWidth = (int)(windowHeight * widthRatio);
#else
	int screenWidth = GetSystemMetrics(SM_CXSCREEN);
	float heightRatio = 450.0f / 800.0f;
	
	windowWidth = screenWidth * 4 / 5;
	windowHeight = (int)(windowWidth * heightRatio);
#endif
}
