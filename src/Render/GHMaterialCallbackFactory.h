// Copyright Golden Hammer Software
#pragma once

class GHMaterial;

// A class to look at a material and possibly create callbacks for it.
class GHMaterialCallbackFactory
{
public:
    virtual ~GHMaterialCallbackFactory(void) {}
    virtual void createCallbacks(GHMaterial& mat) const = 0;
};
