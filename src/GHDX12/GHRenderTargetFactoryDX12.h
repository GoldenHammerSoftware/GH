#pragma once

#include "Render/GHRenderTargetFactory.h"

class GHRenderDeviceDX12;
class GHMipmapGeneratorDX12;

class GHRenderTargetFactoryDX12 : public GHRenderTargetFactory
{
public:
	GHRenderTargetFactoryDX12(GHRenderDeviceDX12& device, GHMipmapGeneratorDX12& mipGen);

	virtual GHRenderTarget* createRenderTarget(const GHRenderTarget::Config& args) const;
	virtual void resize(GHRenderTarget& target, const GHRenderTarget::Config& args) const;

private:
	GHRenderDeviceDX12& mDevice;
	GHMipmapGeneratorDX12& mMipGen;
};
