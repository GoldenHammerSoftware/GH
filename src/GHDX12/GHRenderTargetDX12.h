#pragma once

#include "Render/GHRenderTarget.h"

class GHRenderDeviceDX12;

class GHRenderTargetDX12 : public GHRenderTarget
{
public:
	GHRenderTargetDX12(GHRenderDeviceDX12& device, const GHRenderTarget::Config& args);
	~GHRenderTargetDX12(void);

	virtual void apply(void) override;
	virtual void remove(void) override;
	virtual GHTexture* getTexture(void) override;

	void resize(const GHRenderTarget::Config& args);
};
