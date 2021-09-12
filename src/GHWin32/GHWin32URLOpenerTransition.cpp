// Copyright Golden Hammer Software
#include "GHWin32URLOpenerTransition.h"
#include "GHPlatform/GHDebugMessage.h"
#include "GHPlatform/win32/GHWin32Include.h"

GHWin32URLOpenerTransition::GHWin32URLOpenerTransition(const char* url)
: mURL(url, GHString::CHT_COPY)
{
}

void GHWin32URLOpenerTransition::activate(void)
{
	// todo.
	GHDebugMessage::outputString("Got a request to open url %s", mURL.getChars());
	ShellExecuteA(NULL, "open", mURL.getChars(), NULL, NULL, SW_SHOWNORMAL);
}
