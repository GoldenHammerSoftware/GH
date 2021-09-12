// Copyright Golden Hammer Software
#include "GHString.h"
#include "GHCharLess.h"
#include "assert.h"
#include <stdarg.h>
#include <stdio.h>

GHString::GHString(void)
{
}

GHString::GHString(const GHString& rhs)
{
	*this = rhs;
}

GHString::GHString(const char* text, CharHandlingType type)
{
	setConstChars(text, type);
}

GHString::~GHString(void)
{
	if (mChars && mOwnsChars) {
        delete [] mChars;
        mChars = 0;
    }
}

GHString& GHString::operator=(const GHString& rhs)
{
	if (rhs.mConstChars)
	{
		setConstChars(rhs.mConstChars, rhs.mOwnsChars ? CHT_COPY : CHT_REFERENCE);
	}
	else
	{
		setChars(rhs.mChars, rhs.mOwnsChars ? CHT_COPY : CHT_REFERENCE);
	}
	return *this;
}

const char* GHString::getChars(void) const
{
	if (mConstChars) return mConstChars;
	return mChars;
}

char* GHString::getNonConstChars(void)
{
	transitionToWriteable();
    return mChars;
}

bool GHString::operator ==(const char* other) const
{
	const char* chars = getChars();
	if (!chars && !other) return true;
	if (!chars || !other) return false;

	size_t otherLen = ::strlen(other);
	if (otherLen == mCharLen) 
	{
		return (!::strcmp(other, chars));
	}
	return false;
}

bool GHString::operator ==(const GHString& other) const
{
	const char* chars = getChars();
	const char* otherChars = other.getChars();
	if (!chars && !other) return true;
	if (!chars || !otherChars) return false;

	if (other.getCharLen() == mCharLen) 
	{
		return (!::strcmp(otherChars, chars));
	}
	return false;
}

bool GHString::operator !=(const GHString& other) const
{
	return !(*this == other);
}

bool GHString::operator !=(const char* other) const
{
	return !(*this == other);
}

bool GHString::operator < (const GHString& other) const
{
	GHCharLess charLess;
	return charLess.compare(getChars(), getCharLen(), other.getChars(), other.getCharLen());
}

void GHString::clearNonConstChars(void)
{
	if (!mChars) return;
	if (mOwnsChars) delete [] mChars;
	mChars = 0;
}

void GHString::setConstChars(const char* chars, CharHandlingType type)
{
	clearNonConstChars();
	mConstChars = 0;
	if (!chars) return;

	mCharLen = ::strlen(chars);
	// todo? should cht_claim set mOwnsChars = true for const chars?
	if (type == CHT_REFERENCE || type == CHT_CLAIM) 
	{
		mConstChars = chars;
		mOwnsChars = false;
	}
	else if (type == CHT_COPY) 
	{
		const size_t BUF_SZ = mCharLen + 1;
		mChars = new char[BUF_SZ];
		::strncpy(mChars, chars, BUF_SZ);
		mOwnsChars = true;
	}
    else assert(1==0);
}

void GHString::setChars(char* chars, CharHandlingType type)
{
	clearNonConstChars();
	mConstChars = 0;
	if (!chars) return;

	mCharLen = ::strlen(chars);
	if (type == CHT_REFERENCE) 
	{
		mChars = chars;
		mOwnsChars = false;
	}
	else if (type == CHT_CLAIM) 
	{
		mChars = chars;
		mOwnsChars = true;
	}
	else if (type == CHT_COPY) 
	{
		mChars = new char[mCharLen + 1];
		::strcpy(mChars, chars);
		mOwnsChars = true;
	}
    else assert(1==0);
}

void GHString::format(const char* str, ...)
{
	if (!str) return;
	
	{
		//scoping so buffer doesn't allocate if string is empty
		//if strings are getting cut off, increase this buffer size
		const int bufSize = 8192;
		char buffer[bufSize];
		va_list args;
		va_start(args, str);
		vsnprintf(buffer, bufSize, str, args);
		va_end(args);
		setConstChars(buffer, CHT_COPY);
	}
}

bool GHString::isEmpty(void) const
{
    const char* chars = getChars();
    if (!chars) return true;
    if (chars[0] == '\0') return true;
    return false;
}

void GHString::truncateChars(const char* chars, size_t maxLen)
{
	clearNonConstChars();
	mConstChars = 0;

	char* truncStr = new char[maxLen + 1];
	memset(truncStr, 0, (int)maxLen + 1);
	::snprintf(truncStr, maxLen, "%s", chars);
	assert(maxLen > 3);
	if (truncStr[maxLen] != 0) {
		truncStr[maxLen] = 0;
		truncStr[maxLen - 1] = '.';
		truncStr[maxLen - 2] = '.';
		truncStr[maxLen - 3] = '.';
	}
	setChars(truncStr, GHString::CHT_CLAIM);
}

char* GHString::replace(const char* src, const char* dst, char* searchStart)
{
	transitionToWriteable();
	char* start = strstr(searchStart, src);
	if (!start)
	{
		return 0;
	}

	size_t srcLen = strlen(src);
	size_t dstLen = strlen(dst);
	// This is going to be really inefficient if dstLen > srcLen.  Need to reserve in advance if this is common.
	char* outputChars = mChars;
	if (dstLen > srcLen)
	{
		mCharLen = mCharLen + (dstLen-srcLen);
		char* newChars = new char[mCharLen+1];
		outputChars = newChars;
	}

	char* rebuf = start + srcLen;
	size_t rebufOffset = (rebuf - mChars);
	char* ret = outputChars + rebufOffset;
	*start = '\0';
	sprintf(outputChars, "%s%s%s", mChars, dst, rebuf);
	if (outputChars != mChars)
	{
		// can assume we own chars because of transitionToWriteable.
		delete[] mChars;
		mChars = outputChars;
	}
	mCharLen = strlen(mChars);
	return ret;
}

bool GHString::replaceAll(const char* src, const char* dst)
{
	char* nextSearch = mChars;
	bool replaced = false;
	while (nextSearch != 0) 
	{
		nextSearch = replace(src, dst, nextSearch);
		replaced = true;
	}
	return replaced;
}

void GHString::transitionToWriteable(void)
{
	if (!mConstChars)
	{
		// already writeable.
		return;
	}
	mChars = new char[mCharLen + 1];
	sprintf(mChars, mConstChars, mCharLen);
	mChars[mCharLen] = '\0';
	mOwnsChars = true;
	mConstChars = 0;
}
