// Copyright Golden Hammer Software
#pragma once

// uncomment DEBUG_IDENTIFIERS to store the string as well as the numeric id.
#ifndef GHRETAIL
    #define DEBUG_IDENTIFIERS 1
    #ifdef DEBUG_IDENTIFIERS
        #include <string.h>
		#include <stdio.h>
    #endif
#endif

// An efficient comparison-only identifier.
class GHIdentifier
{
public:
    typedef unsigned long IDType;
    
public:
    GHIdentifier(void) : mId(0)
	{
#ifdef DEBUG_IDENTIFIERS
		mString[0] = '\0';
#endif
	}
    GHIdentifier(IDType id) : mId(id) 
	{
#ifdef DEBUG_IDENTIFIERS
		mString[0] = '\0';
#endif
	}
    GHIdentifier(const GHIdentifier& other)
    { 
        *this = other;
    }
    
    bool operator<(const GHIdentifier& other) const {
        return mId < other.mId;
    }
    bool operator==(const GHIdentifier& other) const {
        return mId == other.mId;
    }
    bool operator!=(const GHIdentifier& other) const { 
        return !(*this == other);
    }
    void operator=(const GHIdentifier& other) 
    { 
        mId = other.mId;
#ifdef DEBUG_IDENTIFIERS
        setString(other.mString);
#endif
    }
    operator IDType(void) const { return mId; }
    
    bool isNull(void) const { return mId == 0; }
    
private:
    IDType mId;

#ifdef DEBUG_IDENTIFIERS
public:
    void setString(const char* str) {
        ::strncpy(mString, str, 63);
    }
    const char* getString(void) const { 
		if (mString[0] == '\0')
		{
			::snprintf((char*)mString, 63, "%d", (int)mId);
		}
		return mString; 
	}
private:
    char mString[64];
#else
public:
    const char* getString(void) const { return "Unknown"; }
#endif
};
