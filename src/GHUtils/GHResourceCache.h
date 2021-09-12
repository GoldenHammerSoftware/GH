// Copyright Golden Hammer Software
#pragma once

#include "GHString/GHIdentifier.h"
#include <map>

class GHResource;

// stores resources that we have loaded for sharing.
class GHResourceCache
{
public:
    ~GHResourceCache(void);
    
    GHResource* getResource(const GHIdentifier& id);
    void storeResource(const GHIdentifier& id, GHResource* resource);
    void removeResource(const GHIdentifier& id);

    // kick anything out of the cache that only has 1 ref count.
    void garbageCollect(void);

private:
    // map of file name to cached resource.
	// todo: switch to unordered_map
    typedef std::map<GHIdentifier, GHResource*> ResourceMap;
    ResourceMap mResources;
};

