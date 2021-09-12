// Copyright Golden Hammer Software
#pragma once

#include <cstdlib>
#include <cwchar>
#include <assert.h>

// place to convert from char* to wide char
class GHLChar
{
public:
	static void convertToWide(const char* src, wchar_t* ret, size_t retLen)
	{
		mbsrtowcs(ret, &src, retLen, NULL);
	}

	static wchar_t* convertToWide(const char* src)
	{
		size_t wideLen = mbsrtowcs(NULL, &src, 0, NULL);
		assert(wideLen != -1);

		wchar_t * buf = new wchar_t[wideLen + 1]();
		wideLen = mbsrtowcs(buf, &src, wideLen + 1, NULL);
		assert(wideLen != -1);

		// caller must delete buf
		return buf;
	}
};
