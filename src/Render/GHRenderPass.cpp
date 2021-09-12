// Copyright Golden Hammer Software
#include "GHRenderPass.h"
#include "GHCamera.h"
#include "GHRenderDevice.h"
#include "GHScreenInfo.h"
#include "GHRenderTarget.h"
#include "GHUtils/GHProfiler.h"

GHIdentifier GHRenderPass::sMainPassId;

GHRenderPass::GHRenderPass(const GHCamera& camera, const GHScreenInfo& screenInfo,
                           GHRenderTarget* renderTarget, GHRenderDevice& device,
                           bool clearBuffers, const GHPoint4* clearColor,
                           const GHIdentifier& id, bool supportsStereo, bool isRenderToTexture)
: mCamera(camera)
, mRenderDevice(device)
, mRenderList(500)
, mScreenInfo(screenInfo)
, mRenderTarget(renderTarget)
, mClearBuffers(clearBuffers)
, mId(id)
, mSupportsStereo(supportsStereo)
, mIsRenderToTexture(isRenderToTexture)
{
    if (clearColor) mClearColor = *clearColor;
}

GHRenderPass::~GHRenderPass(void)
{
	for (size_t i = 0; i < mPredicates.size(); ++i)
	{
		delete mPredicates[i];
	}
}

void GHRenderPass::prepareForCollect(void)
{
	if (!shouldRenderFrame())
	{
		return;
	}

    mViewInfos[GHViewInfo::EYE_MONOCLE].updateView(mCamera, mScreenInfo.getAspectRatio(), mRenderDevice, mViewInfos[GHViewInfo::EYE_LEFT], mViewInfos[GHViewInfo::EYE_RIGHT], mIsRenderToTexture);

    GHPoint3 camPos;
    mCamera.getTransform().getTranslate(camPos);
	mRenderList.initPass(camPos, mViewInfos, mId, mSupportsStereo);
}

void GHRenderPass::draw(void)
{
	if (!shouldRenderFrame())
	{
		return;
	}

	mRenderDevice.beginRenderPass(mRenderTarget, mClearColor, mClearBuffers);
	mRenderList.flushToDevice(mRenderDevice);
	mRenderDevice.endRenderPass();
}

void GHRenderPass::clear(void)
{
	mRenderList.clear();
}

bool GHRenderPass::shouldRenderFrame(void) const
{
	for (size_t i = 0; i < mPredicates.size(); ++i)
	{
		if (!mPredicates[i]->eval())
		{
			return false;
		}
	}
	return true;
}
