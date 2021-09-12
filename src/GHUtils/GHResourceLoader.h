// Copyright Golden Hammer Software
#pragma once

#include <assert.h>
#include <stddef.h> // size_t

class GHFile;
class GHResource;
class GHPropertyContainer;

// Main interface for something that loads resources.
// Generally we have one of these for each supported file extension.
class GHResourceLoader
{
public:
    virtual ~GHResourceLoader(void) {}

	// there are issues with some loaders that make having a const interface be difficult.

	// Take a filename and load a GHResource from the contents.  Returns 0 if file not found.
    virtual GHResource* loadFile(const char* filename, GHPropertyContainer* extraData=0) = 0;

	// Take a memory buffer and decode it into a GHResource.
	// This should probably be pure virtual but it was a late addition to the engine, 
	// so rather than update all loaders on all platforms we just assert if it is not implemented.
	virtual GHResource* loadMemory(void* mem, size_t memSize, GHPropertyContainer* extraData = 0) 
	{
		assert(false && "GHResourceLoader::loadMemory called but not implemented");
		return 0;
	}
};
