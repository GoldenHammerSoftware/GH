// Copyright Golden Hammer Software
#include "GHWin32OutputPipe.h"
#include "GHWin32Include.h"
#include "GHLChar.h"
#include <fstream>

void GHWin32OutputPipe::outputString(const char* str)
{
	wchar_t* wideStr = GHLChar::convertToWide(str);
	::OutputDebugString(wideStr);
	::OutputDebugString(L"\n");

	#if 0
		std::ofstream outFile;
		outFile.open("ghdebugoutput.txt", std::ios_base::app);
		outFile << str << '\n';
	#endif

	delete wideStr;
}

