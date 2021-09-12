// Copyright Golden Hammer Software
#pragma once

#include "GHString/GHIdentifier.h"
#include "GHPlatform/GHRefCounted.h"

class GHResourceCache;

// Interface for something loaded from the resource cache.
class GHResource : public GHRefCounted
{
public:
    GHResource(void);
    
    // Assign a resource id to this object.
    // If a cache is included, release removes the object from the cache on delete.
    // If called with a cache, it is not valid to re-assign the resource id later.
    void assignId(GHIdentifier resourceId, GHResourceCache* cache);
  
    const GHIdentifier& getResourceId(void) const { return mResourceId; }
    
protected:
    virtual ~GHResource(void);
    
private:
    GHResourceCache* mResourceCache;
    GHIdentifier mResourceId;
};

// class for wrapping any arbitrary type in a resource
template< typename T >
class GHResourcePtr : public GHResource
{
public:
	GHResourcePtr(T* obj) : mObj(obj) {}
    
    T* get(void) { return mObj; }
    const T* get(void) const { return mObj; }
    
protected:
    ~GHResourcePtr(void) { delete mObj; }
    
protected:
	T* mObj;
};
