// Copyright 2010 Golden Hammer Software
#include "GHMetroOutputPipe.h"
#include <string.h>
#include <windows.h>

void GHMetroOutputPipe::outputString(const char* str)
{
	if (!str) return;

	size_t len = ::strlen(str)+1;
	wchar_t* wstr = new wchar_t[len];
	::memset(wstr, 0, sizeof(wchar_t)*len);
	MultiByteToWideChar(CP_ACP, 0, str, len, wstr, len);
	::OutputDebugString(wstr);
	::OutputDebugString(L"\n");
	delete [] wstr;
}
