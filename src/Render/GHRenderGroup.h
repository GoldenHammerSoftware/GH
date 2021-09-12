// Copyright Golden Hammer Software
#pragma once

#include "GHRenderable.h"
#include "GHString/GHIdentifier.h"

// a Renderable which contains a set of other renderables.
class GHRenderGroup : public GHRenderable
{
public:
    virtual void addRenderable(GHRenderable& renderable, const GHIdentifier& type) = 0;
    virtual void removeRenderable(GHRenderable& renderable, const GHIdentifier& type) = 0;
};
