// Copyright 2010 Golden Hammer Software
#pragma once

class GHMessageHandler;

namespace PhoneDirect3DXamlAppComponent
{

public delegate void VoidEventHandler();
public delegate void IAPEventHandler(Platform::String^ arg, bool isTrial);

// A class to launch events that C# code can listen to.
public ref class GHWPEventInterface sealed
{
public:
	void requestAdditionalFrame(void) { RequestAdditionalFrame(); }
	event VoidEventHandler^ RequestAdditionalFrame;

	void hideAds(void) { HideAds(); }
	event VoidEventHandler^ HideAds;

	void showAds(void) 
	{ 
		ShowAds(); 
	}
	event VoidEventHandler^ ShowAds;

	// buyIAP tries to open the store for purchasing
	void buyIAP(Platform::String^ name, bool isTrial) { BuyIAP(name, isTrial); }
	event IAPEventHandler^ BuyIAP;
	// confirmIAP just looks at the local license.
	void confirmIAP(Platform::String^ name, bool isTrial) { ConfirmIAP(name, isTrial); }
	event IAPEventHandler^ ConfirmIAP;
};

};
