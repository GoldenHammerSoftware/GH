#pragma once

#include "Render/GHRenderTargetFactory.h"

class GHRenderDeviceDX12;

class GHRenderTargetFactoryDX12 : public GHRenderTargetFactory
{
public:
	GHRenderTargetFactoryDX12(GHRenderDeviceDX12& device);

	virtual GHRenderTarget* createRenderTarget(const GHRenderTarget::Config& args) const;
	virtual void resize(GHRenderTarget& target, const GHRenderTarget::Config& args) const;

private:
	GHRenderDeviceDX12& mDevice;
};
