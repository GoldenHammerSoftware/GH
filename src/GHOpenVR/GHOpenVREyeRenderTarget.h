#pragma once

#include "GHRenderTarget.h"

class GHRenderDevice;
class GHRenderTargetDX11;
class GHTextureDX11;

class GHOpenVREyeRenderTarget : public GHRenderTarget
{
public:
	GHOpenVREyeRenderTarget(GHRenderDevice& renderDevice, GHRenderTargetDX11* renderTarget);
	virtual ~GHOpenVREyeRenderTarget(void);

	virtual void apply(void);
	virtual void remove(void);

	virtual GHTexture* getTexture(void);

	GHTextureDX11* getTextureDX11(void);
	const GHTextureDX11* getTextureDX11(void) const;
	
private:
	GHRenderDevice& mRenderDevice;
	GHRenderTargetDX11* mRenderTarget;
	bool mWasStereo{ false };
};
