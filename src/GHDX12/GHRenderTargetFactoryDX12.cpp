#include "GHRenderTargetFactoryDX12.h"
#include "GHRenderTargetDX12.h"

GHRenderTargetFactoryDX12::GHRenderTargetFactoryDX12(GHRenderDeviceDX12& device)
	: mDevice(device)
{
}

GHRenderTarget* GHRenderTargetFactoryDX12::createRenderTarget(const GHRenderTarget::Config& args) const
{
	return new GHRenderTargetDX12(mDevice, args);
}

void GHRenderTargetFactoryDX12::resize(GHRenderTarget& target, const GHRenderTarget::Config& args) const
{
	((GHRenderTargetDX12&)target).resize(args);
}
