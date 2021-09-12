// Copyright 2010 Golden Hammer Software
#include "GHMetroURLOpenerTransition.h"
#include <string.h>
#include <wrl.h>

GHMetroURLOpenerTransition::GHMetroURLOpenerTransition(const char* url)
{
	size_t len = strlen(url)+1;
	wchar_t* wstr = new wchar_t[len];
	::memset(wstr, 0, sizeof(wchar_t)*len);
	MultiByteToWideChar(CP_ACP, 0, url, len-1, wstr, len);
	mURLString = ref new Platform::String(wstr);
}

void GHMetroURLOpenerTransition::activate(void)
{
	auto uri = ref new Windows::Foundation::Uri(mURLString);
	Windows::System::Launcher::LaunchUriAsync(uri);
}
