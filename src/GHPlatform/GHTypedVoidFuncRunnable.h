// Copyright Golden Hammer Software
#pragma once

#include "GHRunnable.h"

// A templated runnable to call an arbitrary function(void) on an object.
// new GHTypedVoidFuncRunnable<Thing>(thing, &Thing::function);
template<typename T>
class GHTypedVoidFuncRunnable : public GHRunnable
{
public:
    typedef void (T::*VoidFuncPtr)(void);
    
    GHTypedVoidFuncRunnable(T& thing, VoidFuncPtr function)
    : mThing(thing), mVoidFuncPtr(function)
    {
    }
    
    virtual void run(void)
    {
        (mThing.*mVoidFuncPtr)();
    }
    
private:
    VoidFuncPtr mVoidFuncPtr;
    T& mThing;
};
