// Copyright Golden Hammer Software
#pragma once

#include "GHRenderTarget.h"

// platform override interface for creating a render target.
class GHRenderTargetFactory
{
public:
    virtual ~GHRenderTargetFactory(void) {}
    virtual GHRenderTarget* createRenderTarget(const GHRenderTarget::Config& args) const = 0;
	virtual void resize(GHRenderTarget& target, const GHRenderTarget::Config& args) const = 0;
};
