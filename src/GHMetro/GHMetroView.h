// Copyright 2010 Golden Hammer Software
#pragma once

#include "pch.h"
#include <wrl/client.h>
#include "GHPlatform/uwp/GHMetroOutputPipe.h"
#include "GHKeyDef.h"
#include "GHMetroSystemServices.h"

class GHMetroAppLauncher;
class GHApp;

ref class GHMetroView sealed : public Windows::ApplicationModel::Core::IFrameworkView
{
public:
	GHMetroView(void);
    
    // IFrameworkView Methods
    virtual void Initialize(Windows::ApplicationModel::Core::CoreApplicationView^ applicationView);
    virtual void SetWindow(Windows::UI::Core::CoreWindow^ window);
    virtual void Load(Platform::String^ entryPoint);
    virtual void Run();
    virtual void Uninitialize();

protected:
    // Event Handlers
    void OnWindowSizeChanged(Windows::UI::Core::CoreWindow^ sender, Windows::UI::Core::WindowSizeChangedEventArgs^ args);
    void OnLogicalDpiChanged(Platform::Object^ sender);
    void OnActivated(Windows::ApplicationModel::Core::CoreApplicationView^ applicationView, Windows::ApplicationModel::Activation::IActivatedEventArgs^ args);
    void OnSuspending(Platform::Object^ sender, Windows::ApplicationModel::SuspendingEventArgs^ args);
    void OnResuming(Platform::Object^ sender, Platform::Object^ args);
    void OnWindowClosed(Windows::UI::Core::CoreWindow^ sender, Windows::UI::Core::CoreWindowEventArgs^ args);
	void OnVisibilityChanged(Windows::UI::Core::CoreWindow^ sender, Windows::UI::Core::VisibilityChangedEventArgs^ args);
	void OnWindowActivationChanged(Windows::UI::Core::CoreWindow^ sender, Windows::UI::Core::WindowActivatedEventArgs^ args);
	//void OnSettingsPaneCommandsRequested(Windows::UI::ApplicationSettings::SettingsPane^ settingsPane, Windows::UI::ApplicationSettings::SettingsPaneCommandsRequestedEventArgs^ eventArgs);
	void OnOptionsCommandInvoked(Windows::UI::Popups::IUICommand^ command);
	void updateSidebarPause(void);

	void OnPointerPressed(Windows::UI::Core::CoreWindow^ sender, Windows::UI::Core::PointerEventArgs^ args);
    void OnPointerReleased(Windows::UI::Core::CoreWindow^ sender, Windows::UI::Core::PointerEventArgs^ args);
    void OnPointerMoved(Windows::UI::Core::CoreWindow^ sender, Windows::UI::Core::PointerEventArgs^ args);
    void OnKeyDown(Windows::UI::Core::CoreWindow^ sender, Windows::UI::Core::KeyEventArgs^ args);
    void OnKeyUp(Windows::UI::Core::CoreWindow^ sender, Windows::UI::Core::KeyEventArgs^ args);

private:
	GHKeyDef::Enum convertKey(Windows::System::VirtualKey key) const;
	int getGHPointerID(Windows::UI::Core::PointerEventArgs^ args) const;
	int registerWindowsPointer(Windows::UI::Core::PointerEventArgs^ args);
	void unregisterWindowsPointer(Windows::UI::Core::PointerEventArgs^ args);

private:
	GHMetroAppLauncher* mAppLauncher;
	GHApp* mApp;
	unsigned int mWindowsPointerIds[GHMetroSystemServices::sMaxPointers];

    bool m_windowClosed;
	// we want to delay activating the core window until a frame is drawn
	// so we set a flag when OnActivated is called, and do it in the run loop.
	bool mWindowActivated;
	bool mOptionsMenuWasInvoked;
	bool mIsInPausedState;
	int mPauseCounter;
};
