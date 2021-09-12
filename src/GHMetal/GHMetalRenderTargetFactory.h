#pragma once

#include "GHRenderTargetFactory.h"

class GHMetalRenderDevice;

class GHMetalRenderTargetFactory : public GHRenderTargetFactory
{
public:
    GHMetalRenderTargetFactory(GHMetalRenderDevice& device);
    
    virtual GHRenderTarget* createRenderTarget(const GHRenderTarget::Config& args) const;
    virtual void resize(GHRenderTarget& target, const GHRenderTarget::Config& args) const;
    
private:
    GHMetalRenderDevice& mDevice;
};

