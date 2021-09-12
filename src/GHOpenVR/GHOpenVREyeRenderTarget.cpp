#include "GHOpenVREyeRenderTarget.h"
#include "GHRenderDevice.h"
#include "GHRenderTargetDX11.h"

GHOpenVREyeRenderTarget::GHOpenVREyeRenderTarget(GHRenderDevice& renderDevice,
												 GHRenderTargetDX11* renderTarget)
	: mRenderDevice(renderDevice)
	, mRenderTarget(renderTarget)
{

}

GHOpenVREyeRenderTarget::~GHOpenVREyeRenderTarget(void)
{
	delete mRenderTarget;
}

void GHOpenVREyeRenderTarget::apply(void)
{
	mWasStereo = mRenderDevice.getIsStereo();
	mRenderDevice.setIsStereo(true);
	mRenderTarget->apply();
}

void GHOpenVREyeRenderTarget::remove(void)
{
	mRenderTarget->remove();
	mRenderDevice.setIsStereo(false);
}

GHTexture* GHOpenVREyeRenderTarget::getTexture(void)
{
	return mRenderTarget->getTexture();
}

const GHTextureDX11* GHOpenVREyeRenderTarget::getTextureDX11(void) const
{
	return mRenderTarget->getTextureDX11();
}

GHTextureDX11* GHOpenVREyeRenderTarget::getTextureDX11(void)
{ 
	return mRenderTarget->getTextureDX11(); 
}