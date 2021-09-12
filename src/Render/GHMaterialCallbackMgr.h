// Copyright Golden Hammer Software
#pragma once

#include "GHMaterialCallbackFactory.h"
#include <vector>

// A class to store a bunch of GHMaterialCallbackFactories that all get applied.
class GHMaterialCallbackMgr : public GHMaterialCallbackFactory
{
public:
    ~GHMaterialCallbackMgr(void);
    
    virtual void createCallbacks(GHMaterial& mat) const;
    
    // take ownership of a factory.
    virtual void addFactory(GHMaterialCallbackFactory* factory);
    virtual void removeFactory(GHMaterialCallbackFactory* factory);
    
private:
    std::vector<GHMaterialCallbackFactory*> mFactories;
};
