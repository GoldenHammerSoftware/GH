// Copyright Golden Hammer Software
#pragma once

#include "GHDeletionHandle.h"
#include "GHRefCounted.h"

// deletion handle that wraps a GHRefCounted release.
// GHRefCounted has a protected destructor so can't go in a GHTypedDeletionHandle
class GHRefCountedDeletionHandle : public GHDeletionHandle
{
public:
    GHRefCountedDeletionHandle(GHRefCounted* obj) : mObj(obj) {}
	virtual ~GHRefCountedDeletionHandle(void) { mObj->release(); }
    
private:
    GHRefCounted* mObj;
};
