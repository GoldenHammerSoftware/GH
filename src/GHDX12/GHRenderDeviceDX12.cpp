#include "GHRenderDeviceDX12.h"

GHRenderDeviceDX12::GHRenderDeviceDX12(void)
{
}

void GHRenderDeviceDX12::reinit(void)
{
	// todo.
}

bool GHRenderDeviceDX12::beginFrame(void)
{
	// todo
	return true;
}

void GHRenderDeviceDX12::endFrame(void)
{
	// todo
}

void GHRenderDeviceDX12::beginRenderPass(GHRenderTarget* optionalTarget, const GHPoint4& clearColor, bool clearBuffers)
{
	// todo
}

void GHRenderDeviceDX12::endRenderPass(void)
{
	// todo
}

void GHRenderDeviceDX12::applyViewInfo(const GHViewInfo& viewInfo)
{
	// todo
}

void GHRenderDeviceDX12::createDeviceViewTransforms(const GHViewInfo::ViewTransforms& engineTransforms,
	GHViewInfo::ViewTransforms& deviceTransforms,
	const GHCamera& camera, bool isRenderToTexture) const
{
	// todo
}

const GHViewInfo& GHRenderDeviceDX12::getViewInfo(void) const
{
	return mViewInfo;
}

void GHRenderDeviceDX12::handleGraphicsQualityChange(void)
{
	// todo
}

GHTexture* GHRenderDeviceDX12::resolveBackbuffer(void)
{
	// todo
	return 0;
}
