// Copyright Golden Hammer Software
#pragma once

// A little util class to store unknown objects for later deletion.
// Keep a list of GHDeletionHandle*, 
//  pass it GHTypedDeletionHandle<Specifics>,
//  and then delete the base type while templates handle the rest.

class GHDeletionHandle
{
public:
	virtual ~GHDeletionHandle(void) {}
};

template< typename T >
class GHTypedDeletionHandle : public GHDeletionHandle
{
public:
	GHTypedDeletionHandle(T* obj) : mObj(obj) {}
	~GHTypedDeletionHandle(void) { delete mObj; }

	operator const T*(void) const { return mObj; }
	operator T*(void) { return mObj; }

protected:
	T* mObj;
};

template< typename T >
class GHArrayDeletionHandle : public GHDeletionHandle
{
public:
    GHArrayDeletionHandle(T* arr) : mArr(arr) { }
    ~GHArrayDeletionHandle(void) { delete[] mArr; }
    
    operator const T*(void) const { return mArr; }
    operator T*(void) { return mArr; }
    
protected:
    T* mArr;
};
