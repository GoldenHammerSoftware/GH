#include "GHOculusRenderTarget.h"
#include "GHPlatform/GHDebugMessage.h"
#include "GHOculusUtil.h"

GHOculusRenderTarget::GHOculusRenderTarget(ovrSession session,
	const ovrSizei& leftSize, const ovrSizei& rightSize)
	: mSession(session)
	, mWidth(leftSize.w + rightSize.w)
	, mHeight(max(leftSize.h, rightSize.h))
{
	mEyeViewports[ovrEye_Left].Pos.x = mEyeViewports[ovrEye_Left].Pos.y = 0;
	mEyeViewports[ovrEye_Left].Size = leftSize;

	mEyeViewports[ovrEye_Right].Pos.x = leftSize.w;
	mEyeViewports[ovrEye_Right].Pos.y = 0;
	mEyeViewports[ovrEye_Right].Size = rightSize;
}

GHOculusRenderTarget::~GHOculusRenderTarget(void)
{
	if (mSwapChain)
	{
		ovr_DestroyTextureSwapChain(mSession, mSwapChain);
	}
}
