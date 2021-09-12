// Copyright 2010 Golden Hammer Software
#include "GHMetroThreadSleeper.h"
#include <windows.h>

void GHMetroThreadSleeper::sleep(int ms)
{
	WaitForSingleObjectEx(GetCurrentThread(), ms, FALSE);
}
