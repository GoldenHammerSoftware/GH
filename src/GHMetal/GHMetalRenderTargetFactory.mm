#include "GHMetalRenderTargetFactory.h"
#include "GHMetalRenderTarget.h"

GHMetalRenderTargetFactory::GHMetalRenderTargetFactory(GHMetalRenderDevice& device)
: mDevice(device)
{
}

GHRenderTarget* GHMetalRenderTargetFactory::createRenderTarget(const GHRenderTarget::Config& args) const
{
    return new GHMetalRenderTarget(args, mDevice);
}

void GHMetalRenderTargetFactory::resize(GHRenderTarget& target, const GHRenderTarget::Config& args) const
{
    GHMetalRenderTarget* metalTarget = (GHMetalRenderTarget*)&target;
    metalTarget->resize(args);
}
