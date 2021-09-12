// Copyright Golden Hammer Software
#include "GHRefCounted.h"

GHRefCounted::GHRefCounted(void)
: mRefCount(0)
{
}

void GHRefCounted::acquire(void)
{
    mRefCount++;
}

void GHRefCounted::release(void)
{
    mRefCount--;
    if (mRefCount <= 0)
    {
        delete this;
    }
}

void GHRefCounted::changePointer(GHRefCounted*& destPtr, GHRefCounted* newPtr)
{
    if (newPtr) newPtr->acquire();
    if (destPtr) destPtr->release();
    destPtr = newPtr;
}

