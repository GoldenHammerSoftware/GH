#pragma once
#ifdef GH_DX12

#include "GHOculusRenderTarget.h"

class GHRenderDeviceDX12;

class GHOculusDX12RenderTarget : public GHOculusRenderTarget
{
public:
	GHOculusDX12RenderTarget(ovrSession session, GHRenderDeviceDX12& ghRenderDevice, const ovrSizei& leftSize, const ovrSizei& rightSize);
	~GHOculusDX12RenderTarget(void);

	// make the target the place we render to.
	virtual void apply(void) override;
	// switch rendering back to how it was before apply.
	virtual void remove(void) override;

	virtual GHTexture* getTexture(void) override;

	virtual void commitChanges(void) override;

private:
	GHRenderDeviceDX12& mGHRenderDevice;
};

#endif

