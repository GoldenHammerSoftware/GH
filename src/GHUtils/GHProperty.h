// Copyright Golden Hammer Software
#pragma once

#include "GHPlatform/GHStaticAssert.h"
#include <string.h>
#include "GHPlatform/GHRefCounted.h"
#include <memory.h>

// A property is a generic chunk of data pushed into a void*.
// All numbers and bools are treated as floats.
// We expect the user to know what kind of data to expect.
// If it's a POD type, we just copy the value into the pointer.
//  POD types larger than void* are not supported.
// Otherwise, we can optionally store a deletion handle to own the value.
class GHProperty
{
public:
    GHProperty(void)
    : mVal(0)
    , mRefCounted(0)
    {}
    
    GHProperty(const GHProperty& rhs)
    : mVal(rhs.mVal)
    , mRefCounted(rhs.mRefCounted)
    {
        if (mRefCounted) mRefCounted->acquire();
    }
    
    template<typename T>
    GHProperty(T val, GHRefCounted* refCounted)
    : mVal(0)
    , mRefCounted(refCounted)
    {
        setVal<T>(val);
        if (mRefCounted) mRefCounted->acquire();
    }
        
    template<typename T>
    GHProperty(T val)
    : mRefCounted(0)
    {
        setVal<T>(val);
    }

    ~GHProperty(void)
    {
        if (mRefCounted) mRefCounted->release();
    }

    GHRefCounted* getRefCounted(void) { return mRefCounted; }

    template<typename T>
	const T getVal(void) const
	{
		GHSTATICASSERT(sizeof(T) <= sizeof(void*));
		return (T)(*this);
	}
    template<typename T>
	T getVal(void)
	{
		GHSTATICASSERT(sizeof(T) <= sizeof(void*));
		return (T)(*this);
	}
    
 	template<typename T>
	void setVal(const T& val)
	{
		GHSTATICASSERT(sizeof(T) <= sizeof(void*));
        mVal = 0;
		memcpy(&mVal, &val, sizeof(T));
	}

 	operator int(void) const { return (int)getVal<float>(); }
    operator double(void) const { return (double)getVal<float>(); }
    operator short(void) const { return (short)getVal<float>(); }
    operator bool(void) const { return (getVal<float>() != 0.0f); }
    operator float(void) const { return *(float*)(&mVal); }
    operator unsigned int(void) const { return (unsigned int)getVal<float>(); }

    template<typename T>
    operator const T(void) const { return *(T*)(&mVal); }

    bool operator<(const GHProperty& other) const { return mVal < other.mVal; }
    bool operator==(const GHProperty& other) const { return mVal == other.mVal; }
    bool operator!=(const GHProperty& other) const { return mVal != other.mVal; }
    void operator=(const GHProperty& other) 
    {
        GHRefCounted* tmpRef = mRefCounted;
        mVal = other.mVal;
        mRefCounted = other.mRefCounted;
        if (mRefCounted) mRefCounted->acquire();
        if (tmpRef) tmpRef->release();
    }
    
public:
	static const GHProperty sInvalidProperty; // call isValid, don't do direct comparisons to this default value.
	bool isValid(void) const { return this != &sInvalidProperty; }
    
private:
	void* mVal;
    GHRefCounted* mRefCounted;
};


template<>
void GHProperty::setVal(const int& val);

template<>
void GHProperty::setVal(const unsigned int& val);

template<>
void GHProperty::setVal(const short& val);

template<>
void GHProperty::setVal(const double& val);

template<>
void GHProperty::setVal(const bool& val);

