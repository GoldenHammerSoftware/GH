#pragma once

#include "GHRenderTarget.h"
#include "OVR_CAPI_D3D.h"
#include <vector>
#include "GHMath/GHPoint.h"

class GHRenderDeviceDX11;

class GHOculusRenderTarget : public GHRenderTarget
{
public:
	GHOculusRenderTarget(ovrSession session, const ovrSizei& leftSize, const ovrSizei& rightSize);
	~GHOculusRenderTarget(void);

	virtual void commitChanges(void) = 0;

	ovrTextureSwapChain getSwapChain(void) { return mSwapChain; }
	unsigned int getWidth(void) const { return mWidth; }
	unsigned int getHeight(void) const { return mHeight; }
	const ovrRecti& getViewport(int index) { return mEyeViewports[index]; }

protected:
	ovrSession										mSession{ nullptr };
	ovrTextureSwapChain								mSwapChain{ nullptr };
	unsigned int									mWidth;
	unsigned int									mHeight;
	ovrRecti										mEyeViewports[ovrEye_Count];
};
