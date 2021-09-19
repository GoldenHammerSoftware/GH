#pragma once

#include "Render/GHRenderDevice.h"

class GHRenderDeviceDX12 : public GHRenderDevice
{
public:
	GHRenderDeviceDX12(void);

	void reinit(void);

	virtual bool beginFrame(void) override;
	virtual void endFrame(void) override;

	virtual void beginRenderPass(GHRenderTarget* optionalTarget, const GHPoint4& clearColor, bool clearBuffers) override;
	virtual void endRenderPass(void) override;

	virtual void applyViewInfo(const GHViewInfo& viewInfo) override;
	virtual void createDeviceViewTransforms(const GHViewInfo::ViewTransforms& engineTransforms,
		GHViewInfo::ViewTransforms& deviceTransforms,
		const GHCamera& camera, bool isRenderToTexture) const override;
	virtual const GHViewInfo& getViewInfo(void) const override;

	virtual void handleGraphicsQualityChange(void) override;

	virtual GHTexture* resolveBackbuffer(void) override;

protected:
	GHViewInfo mViewInfo;
};

