// Copyright Golden Hammer Software
#pragma once

class GHRenderPass;
class GHFrustum;
class GHPropertyContainer;

// interface for something that can be drawn from a scene renderer.
class GHRenderable
{
public:
    GHRenderable(void) : mDrawOrder(0), mEntityData(0) {}
    virtual ~GHRenderable(void) {}
    
    // Add any low level renderable types to the pass's renderList if appropriate.
    virtual void collect(GHRenderPass& pass, const GHFrustum* frustum) = 0;
    
    void setDrawOrder(float val) { mDrawOrder = val; }
    void setEntityData(const GHPropertyContainer* entData) { mEntityData = entData; }
    
protected:
    float mDrawOrder;
    // optional data blob for everything we pass to the render list.
    const GHPropertyContainer* mEntityData;
};
