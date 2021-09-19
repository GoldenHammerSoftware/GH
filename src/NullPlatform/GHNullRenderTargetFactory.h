// Copyright Golden Hammer Software
#pragma once

#include "Render/GHRenderTargetFactory.h"
#include "Render/GHRenderTarget.h"

class GHNullRenderTarget : public GHRenderTarget
{
public:
    virtual void apply(void) {}
    virtual void remove(void) {}
    virtual GHTexture* getTexture(void) { return 0; }
};

class GHNullRenderTargetFactory : public GHRenderTargetFactory
{
public:
    virtual GHRenderTarget* createRenderTarget(const GHRenderTarget::Config& args) const
    {
        return new GHNullRenderTarget;
    }
	virtual void resize(GHRenderTarget& target, const GHRenderTarget::Config& args) const {}
};
