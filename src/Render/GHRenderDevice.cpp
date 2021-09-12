#include "GHRenderDevice.h"
#include "GHHMDRenderDevice.h"

GHRenderDevice::~GHRenderDevice(void)
{
	delete mHMDDevice;
}

void GHRenderDevice::setHMDDevice(GHHMDRenderDevice* hmd)
{
	if (mHMDDevice) delete mHMDDevice;
	mHMDDevice = hmd;
}

void GHRenderDevice::createStereoView(const GHViewInfo& mainView, const GHCamera& camera,
	GHViewInfo& leftView, GHViewInfo& rightView, GHFrustum& combinedFrustum) const
{
	if (!mHMDDevice || !mHMDDevice->isActive())
	{
		leftView = mainView;
		rightView = mainView;
		combinedFrustum = mainView.getFrustum();
		return;
	}

	mHMDDevice->createStereoView(mainView, camera, leftView, rightView, combinedFrustum);
}

void GHRenderDevice::setIsStereo(bool val)
{
	mIsStereo = val;
}

bool GHRenderDevice::getIsStereo(void) const
{
	return mIsStereo;
}
