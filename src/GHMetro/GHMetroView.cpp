// Copyright 2010 Golden Hammer Software
#include "pch.h"
#include "GHMetroView.h"
#include "GHSpecificAppLauncher.h"
#include "GHApp.h"
#include <cassert>
#include "GHInputState.h"
#include "GHScreenInfo.h"
#include "GHUtils/GHMessageQueue.h"
#include "GHMessageTypes.h"
#include "GHUtils/GHMessage.h"
#include "GHSystemServices.h"

using namespace Windows::ApplicationModel;
using namespace Windows::ApplicationModel::Core;
using namespace Windows::ApplicationModel::Activation;
using namespace Windows::UI::Core;
using namespace Windows::System;
using namespace Windows::Foundation;
using namespace Windows::Graphics::Display;

GHMetroView::GHMetroView(void) 
	: m_windowClosed(false)
	, mAppLauncher(0)
	, mWindowActivated(false)
	, mIsInPausedState(false)
	, mPauseCounter(0)
{
	for (int i = 0; i < GHMetroSystemServices::sMaxPointers; ++i)
	{
		mWindowsPointerIds[i] = 1;
	}
}

void GHMetroView::Initialize(CoreApplicationView^ applicationView)
{
    applicationView->Activated +=
        ref new TypedEventHandler<CoreApplicationView^, IActivatedEventArgs^>(this, &GHMetroView::OnActivated);

    CoreApplication::Suspending +=
        ref new EventHandler<SuspendingEventArgs^>(this, &GHMetroView::OnSuspending);

    CoreApplication::Resuming +=
        ref new EventHandler<Platform::Object^>(this, &GHMetroView::OnResuming);
}

void GHMetroView::SetWindow(CoreWindow^ window)
{
    window->SizeChanged += 
        ref new TypedEventHandler<CoreWindow^, WindowSizeChangedEventArgs^>(this, &GHMetroView::OnWindowSizeChanged);

    window->Closed += 
        ref new TypedEventHandler<CoreWindow^, CoreWindowEventArgs^>(this, &GHMetroView::OnWindowClosed);

    window->PointerCursor = ref new CoreCursor(CoreCursorType::Arrow, 0);

	window->VisibilityChanged +=
		ref new TypedEventHandler<CoreWindow^, VisibilityChangedEventArgs^>(this, &GHMetroView::OnVisibilityChanged);

	window->Activated +=
		ref new TypedEventHandler<CoreWindow^, WindowActivatedEventArgs^>(this, &GHMetroView::OnWindowActivationChanged);

	window->PointerPressed +=
	    ref new TypedEventHandler<CoreWindow^, PointerEventArgs^>(this, &GHMetroView::OnPointerPressed);

    window->PointerReleased +=
        ref new TypedEventHandler<CoreWindow^, PointerEventArgs^>(this, &GHMetroView::OnPointerReleased);

    window->PointerMoved +=
        ref new TypedEventHandler<CoreWindow^, PointerEventArgs^>(this, &GHMetroView::OnPointerMoved);

    window->KeyDown +=
        ref new TypedEventHandler<CoreWindow^, KeyEventArgs^>(this, &GHMetroView::OnKeyDown);

    window->KeyUp +=
        ref new TypedEventHandler<CoreWindow^, KeyEventArgs^>(this, &GHMetroView::OnKeyUp);

	//Windows::UI::ApplicationSettings::SettingsPane^ settingsPane = Windows::UI::ApplicationSettings::SettingsPane::GetForCurrentView();
	//settingsPane->CommandsRequested += 
	//	ref new TypedEventHandler<Windows::UI::ApplicationSettings::SettingsPane^,
	//	Windows::UI::ApplicationSettings::SettingsPaneCommandsRequestedEventArgs^>(this, &GHMetroView::OnSettingsPaneCommandsRequested);


	assert(mAppLauncher == 0);
	// We expect specific apps (bowling etc) to replace GHSpecificAppLauncher.cpp and .h 
	// to include a version that creates the appropriate GHApp.
	mAppLauncher = new GHSpecificAppLauncher(nullptr, nullptr, nullptr);
	mAppLauncher->initialize();
}

void GHMetroView::Load(Platform::String^ entryPoint)
{
}

void GHMetroView::Run()
{
    while (!m_windowClosed)
    {
		mAppLauncher->getSystemServices()->getInputState().clearInputEvents(); // has to be sent before processing new events for this frame.
        CoreWindow::GetForCurrentThread()->Dispatcher->ProcessEvents(CoreProcessEventsOption::ProcessAllIfPresent);
		mAppLauncher->runFrame();

	    if (mWindowActivated && CoreWindow::GetForCurrentThread()) {
			CoreWindow::GetForCurrentThread()->Activate();
			mWindowActivated = false;
		}

		// we delay the pause/unpause until at least one frame is drawn
		// otherwise we could miss it if it happens while loading the app.
		updateSidebarPause();

		if (mOptionsMenuWasInvoked) {
			mAppLauncher->handleOptionsMenuInvoked();
			mOptionsMenuWasInvoked = false;
		}
    }
}

void GHMetroView::updateSidebarPause(void)
{
	// we delay the pause/unpause until at least one frame is drawn
	// otherwise we could miss it if it happens while loading the app.
	if (!mIsInPausedState)
	{
		if (mPauseCounter > 0 ||
			Windows::UI::ViewManagement::ApplicationView::Value == 
				Windows::UI::ViewManagement::ApplicationViewState::Snapped)
		{
			mAppLauncher->getMessageQueue().handleMessage(GHMessage(GHMessageTypes::PAUSEINTERRUPT));
			mIsInPausedState = true;
		}
	}
	else if (mPauseCounter < 0 && mIsInPausedState &&
		Windows::UI::ViewManagement::ApplicationView::Value != 
				Windows::UI::ViewManagement::ApplicationViewState::Snapped)
	{
		mAppLauncher->getMessageQueue().handleMessage(GHMessage(GHMessageTypes::UNPAUSEINTERRUPT));
		mIsInPausedState = false;
	}
	mPauseCounter = 0;
}

void GHMetroView::Uninitialize()
{
	delete mAppLauncher;
}

void GHMetroView::OnWindowSizeChanged(CoreWindow^ sender, WindowSizeChangedEventArgs^ args)
{
	if (!mAppLauncher) return;
	bool widescreen = (args->Size.Width > args->Size.Height);
	mAppLauncher->handleWindowSizeChanged(GHPoint2i(args->Size.Width, args->Size.Height));
}

void GHMetroView::OnWindowClosed(CoreWindow^ sender, CoreWindowEventArgs^ args)
{
    m_windowClosed = true;
}

void GHMetroView::OnActivated(CoreApplicationView^ applicationView, IActivatedEventArgs^ args)
{
	mWindowActivated = true;
}

void GHMetroView::OnSuspending(Platform::Object^ sender, SuspendingEventArgs^ args)
{
    // Save application state after requesting a deferral. Holding a deferral
    // indicates that the application is busy performing suspending operations.
    // Be aware that a deferral may not be held indefinitely. After about five
    // seconds, the application will be forced to exit.
     SuspendingDeferral^ deferral = args->SuspendingOperation->GetDeferral();

     // Insert your code here

     deferral->Complete();
}
 
void GHMetroView::OnResuming(Platform::Object^ sender, Platform::Object^ args)
{
}

void GHMetroView::OnWindowActivationChanged(Windows::UI::Core::CoreWindow^ sender, Windows::UI::Core::WindowActivatedEventArgs^ args)
{
	if (args->WindowActivationState == Windows::UI::Core::CoreWindowActivationState::Deactivated
		|| Windows::UI::ViewManagement::ApplicationView::Value == 
			Windows::UI::ViewManagement::ApplicationViewState::Snapped)
	{
		mPauseCounter = 1;
	}
	else
	{
		mPauseCounter = -1;
	}
}

void GHMetroView::OnVisibilityChanged(Windows::UI::Core::CoreWindow^ sender, Windows::UI::Core::VisibilityChangedEventArgs^ args)
{
	if (!mAppLauncher) return;
}

//void GHMetroView::OnSettingsPaneCommandsRequested(Windows::UI::ApplicationSettings::SettingsPane^ settingsPane, Windows::UI::ApplicationSettings::SettingsPaneCommandsRequestedEventArgs^ eventArgs)
//{
//	Windows::UI::Popups::UICommandInvokedHandler^ handler = ref new Windows::UI::Popups::UICommandInvokedHandler(this, &GHMetroView::OnOptionsCommandInvoked);

//	Windows::UI::ApplicationSettings::SettingsCommand^ gameOptions = ref new Windows::UI::ApplicationSettings::SettingsCommand("gameOptions", "Game Options", handler);
//    eventArgs->Request->ApplicationCommands->Append(gameOptions);
//}

void GHMetroView::OnOptionsCommandInvoked(Windows::UI::Popups::IUICommand^ command)
{
	mOptionsMenuWasInvoked = true;
}

void GHMetroView::OnPointerPressed(Windows::UI::Core::CoreWindow^ sender, Windows::UI::Core::PointerEventArgs^ args)
{
	if (!mAppLauncher) return;

	//For the mouse, pressed/released are handled by way of polling.
	// This is in order to support multiple mouse buttons being pressed simultaneously.
	// The OS does not send events for buttons being pressed/released while other mouse buttons are being held.
	if (args->CurrentPoint->PointerId == 1)
	{
		return;
	}

	int pointerId = registerWindowsPointer(args);
	mAppLauncher->getSystemServices()->getInputState().handlePointerActive(pointerId, true);
	mAppLauncher->getSystemServices()->getInputState().handleKeyChange(0, (int)GHKeyDef::KEY_MOUSE1 + (pointerId*3), true);
}

void GHMetroView::OnPointerReleased(Windows::UI::Core::CoreWindow^ sender, Windows::UI::Core::PointerEventArgs^ args)
{
	if (!mAppLauncher) return;

	//For the mouse, pressed/released are handled by way of polling.
	// This is in order to support multiple mouse buttons being pressed simultaneously.
	// The OS does not send events for buttons being pressed/released while other mouse buttons are being held.
	if (args->CurrentPoint->PointerId == 1)
	{
		return;
	}

	int pointerId = getGHPointerID(args);
	mAppLauncher->getSystemServices()->getInputState().handleKeyChange(0, (int)GHKeyDef::KEY_MOUSE1 + (pointerId*3), false);
	mAppLauncher->getSystemServices()->getInputState().handlePointerActive(pointerId, false);
	unregisterWindowsPointer(args);
}

void GHMetroView::OnPointerMoved(Windows::UI::Core::CoreWindow^ sender, Windows::UI::Core::PointerEventArgs^ args)
{
	if (!mAppLauncher) return;
	Windows::UI::Input::PointerPoint^ pt = args->CurrentPoint;
	int pointerId = getGHPointerID(args);
	if (pointerId >= 0)
	{
		GHPoint2 normPos;
		normPos[0] = pt->Position.X / ((float)mAppLauncher->getScreenInfo().getSize()[0]);
		normPos[1] = pt->Position.Y / ((float)mAppLauncher->getScreenInfo().getSize()[1]);
		mAppLauncher->getSystemServices()->getInputState().handlePointerChange(pointerId, normPos);
	}
}

void GHMetroView::OnKeyDown(Windows::UI::Core::CoreWindow^ sender, Windows::UI::Core::KeyEventArgs^ args)
{
	if (!mAppLauncher) return;
	mAppLauncher->getSystemServices()->getInputState().handleKeyChange(0, convertKey(args->VirtualKey), 1);
}

void GHMetroView::OnKeyUp(Windows::UI::Core::CoreWindow^ sender, Windows::UI::Core::KeyEventArgs^ args)
{
	if (!mAppLauncher) return;
	mAppLauncher->getSystemServices()->getInputState().handleKeyChange(0, convertKey(args->VirtualKey), 0);
}

GHKeyDef::Enum GHMetroView::convertKey(Windows::System::VirtualKey key) const
{
	switch (key)
	{
	case Windows::System::VirtualKey::Up:
		return GHKeyDef::KEY_UPARROW;
	case Windows::System::VirtualKey::Down:
		return GHKeyDef::KEY_DOWNARROW;
	case Windows::System::VirtualKey::Left:
		return GHKeyDef::KEY_LEFTARROW;
	case Windows::System::VirtualKey::Right:
		return GHKeyDef::KEY_RIGHTARROW;
	case Windows::System::VirtualKey::Enter:
		return GHKeyDef::KEY_ENTER;
	case Windows::System::VirtualKey::Shift:
		return GHKeyDef::KEY_SHIFT;
	case Windows::System::VirtualKey::Escape:
		return GHKeyDef::KEY_ESC;
	case Windows::System::VirtualKey::LeftButton:
		return GHKeyDef::KEY_MOUSE1;
	case Windows::System::VirtualKey::RightButton:
		return GHKeyDef::KEY_MOUSE2;
	case Windows::System::VirtualKey::MiddleButton:
		return GHKeyDef::KEY_MOUSE3;
	default:
		return (GHKeyDef::Enum)tolower((int)key);
	};
}

int GHMetroView::getGHPointerID(Windows::UI::Core::PointerEventArgs^ args) const
{
	//special case for mouse pointer - Windows gives it ID 1. We give it ID 0
	if (args->CurrentPoint->PointerId == 1) {
		return 0;
	}

	for (int i = 0; i < GHMetroSystemServices::sMaxPointers; ++i)
	{
		if (mWindowsPointerIds[i] == args->CurrentPoint->PointerId) {
			return i+1;
		}
	}

	//We shouldn't have reached here, unless maybe there were
	// more touches than the game could handle. 
	return -1;
}

int GHMetroView::registerWindowsPointer(Windows::UI::Core::PointerEventArgs^ args)
{
	// first check to see if it is already registered.
	int testId = getGHPointerID(args);
	if (testId > -1) return testId;

	// the mouse is special cased to pointer 1, touch 0.
	if (args->CurrentPoint->PointerId == 1) return 0;

	for (unsigned int i = 0; i < GHMetroSystemServices::sMaxPointers-1; ++i)
	{
		//since 1 is the windows value reserved for the mouse, we
		// don't store it and instead use it as our empty value
		if (mWindowsPointerIds[i] == 1)
		{
			mWindowsPointerIds[i] = args->CurrentPoint->PointerId;
			return i+1; // pointer 0 is the mouse.
		}
	}
	assert(false);
	return 0;
}

void GHMetroView::unregisterWindowsPointer(Windows::UI::Core::PointerEventArgs^ args)
{
	for (unsigned int i = 0; i < GHMetroSystemServices::sMaxPointers; ++i)
	{
		if (mWindowsPointerIds[i] == args->CurrentPoint->PointerId)
		{
			//since 1 is the value reserved for the mouse, we
			// don't store it and instead use it as our empty value
			mWindowsPointerIds[i] = 1;
		}
	}
}
