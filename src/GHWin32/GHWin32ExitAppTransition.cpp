// Copyright Golden Hammer Software
#include "GHWin32ExitAppTransition.h"
#include "GHPlatform/win32/GHWin32Include.h"

void GHWin32ExitAppTransition::activate(void)
{
	PostQuitMessage(0);
}
