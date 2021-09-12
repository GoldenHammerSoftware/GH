// Copyright Golden Hammer Software
#pragma once

class GHMutex
{
public:
	virtual ~GHMutex(void) {}
	
	virtual bool acquire(void) = 0;
	virtual bool release(void) = 0;
};

class GHScopedMutex
{
public:
    GHScopedMutex(GHMutex* mutex): mMutex(mutex) {
        if (mMutex) mMutex->acquire();
    }
    ~GHScopedMutex(void) {
        if (mMutex) mMutex->release();
    }
private:
    GHMutex* mMutex;
};
