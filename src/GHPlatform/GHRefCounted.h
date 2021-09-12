// Copyright Golden Hammer Software
#pragma once

// A class to manage the lifecycle of shared objects.
class GHRefCounted
{
public:
    GHRefCounted(void);
    
    // inc mRefCount
    void acquire(void);
    // del mRefCount.  when it hits 0, delete this object
    void release(void);

    unsigned short getRefCount(void) const { return mRefCount; }

    static void changePointer(GHRefCounted*& destPtr, GHRefCounted* newPtr);
    
protected:
    // we don't want delete called by users.
    // all deletes should come from calls to release();
    virtual ~GHRefCounted(void) {}

private:
    // a count of how many people have called acquire without calling release.
    unsigned short mRefCount;
};

template< typename T >
class GHRefCountedType : public GHRefCounted
{
public:
	GHRefCountedType(T* obj) : mObj(obj) {}

    T* get(void) { return mObj; }
    const T* get(void) const { return mObj; }

protected:
    ~GHRefCountedType(void) { delete mObj; }

protected:
	T* mObj;
};

template< typename T >
class GHRefCountedArrayType : public GHRefCounted
{
public:
	GHRefCountedArrayType(T* obj) : mObj(obj) {}
    
    T* get(void) { return mObj; }
    const T* get(void) const { return mObj; }
    
protected:
    ~GHRefCountedArrayType(void) { delete [] mObj; }
    
protected:
	T* mObj;
};

