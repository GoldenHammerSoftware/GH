#ifdef GH_DX12

#include "GHOculusDX12RenderTarget.h"
#include "GHDX12/GHRenderDeviceDX12.h"
#include "GHPlatform/GHDebugMessage.h"
#include "GHOculusUtil.h"

GHOculusDX12RenderTarget::GHOculusDX12RenderTarget(ovrSession session, GHRenderDeviceDX12& ghRenderDevice, const ovrSizei& leftSize, const ovrSizei& rightSize)
	: GHOculusRenderTarget(session, leftSize, rightSize)
	, mGHRenderDevice(ghRenderDevice) 
{
}

GHOculusDX12RenderTarget::~GHOculusDX12RenderTarget(void)
{
}

void GHOculusDX12RenderTarget::apply(void)
{
}

void GHOculusDX12RenderTarget::remove(void)
{
}

GHTexture* GHOculusDX12RenderTarget::getTexture(void)
{
	return 0;
}

void GHOculusDX12RenderTarget::commitChanges(void)
{
}

#endif
