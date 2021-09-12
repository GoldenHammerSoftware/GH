// Copyright Golden Hammer Software
#pragma once

#include "GHRenderable.h"
#include <vector>

class GHTransformNode;
class GHGeometry;
class GHRunnable;
class GHSphereBounds;

// A renderable for drawing a set of geometry, like a model.
class GHGeometryRenderable : public GHRenderable
{
public:
    GHGeometryRenderable(void);
    ~GHGeometryRenderable(void);
    
    virtual void collect(GHRenderPass& pass, const GHFrustum* frustum);
    
    void addGeometry(GHGeometry* geo);
	void removeGeometry(GHGeometry* geo);
	const std::vector<GHGeometry*>& getGeometry(void) const { return mGeometry; }
    void addBounds(GHSphereBounds& bounds) { mBounds.push_back(&bounds); }
    
    void setRenderCallback(GHRunnable* callback) { mRenderCallback = callback; }
    
private:
    std::vector<GHGeometry*> mGeometry;
    std::vector<GHSphereBounds*> mBounds;
    GHRunnable* mRenderCallback;
};
