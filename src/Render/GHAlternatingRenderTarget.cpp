// Copyright Golden Hammer Software
#include "GHAlternatingRenderTarget.h"
#include <assert.h>

GHAlternatingRenderTarget::GHAlternatingRenderTarget(GHRenderTarget* t1, GHRenderTarget* t2)
: mActiveRenderTarget(t1)
, mPreviousRenderTarget(t2)
{
    assert(mActiveRenderTarget);
    assert(mPreviousRenderTarget);
}

GHAlternatingRenderTarget::~GHAlternatingRenderTarget(void)
{
    delete mActiveRenderTarget;
    delete mPreviousRenderTarget;
}

void GHAlternatingRenderTarget::apply(void)
{
    mActiveRenderTarget->apply();
}

void GHAlternatingRenderTarget::remove(void)
{
    mActiveRenderTarget->remove();
    GHRenderTarget* swap = mActiveRenderTarget;
    mActiveRenderTarget = mPreviousRenderTarget;
    mPreviousRenderTarget = swap;
}

GHTexture* GHAlternatingRenderTarget::getTexture(void)
{
    return mPreviousRenderTarget->getTexture();
}

