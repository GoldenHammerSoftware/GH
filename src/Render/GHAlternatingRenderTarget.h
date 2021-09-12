// Copyright Golden Hammer Software
#pragma once

#include "GHRenderTarget.h"

// a render target that alternates between two backing textures
// this allows us to use the previous frame's pass during this frame's render.
class GHAlternatingRenderTarget : public GHRenderTarget
{
public:
    // we take ownership of t1 and t2
    GHAlternatingRenderTarget(GHRenderTarget* t1, GHRenderTarget* t2);
    ~GHAlternatingRenderTarget(void);
    
	virtual void apply(void);
	virtual void remove(void);
	virtual GHTexture* getTexture(void);

private:
    GHRenderTarget* mActiveRenderTarget;
    GHRenderTarget* mPreviousRenderTarget;
};
