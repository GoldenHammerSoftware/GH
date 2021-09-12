// Copyright 2010 Golden Hammer Software
#include "GHGLESRenderDevice.h"
#include "GHGLESInclude.h"
#include "GHGLESContext.h"
#include "GHPlatform/GHMutex.h"
#include "GHGLESErrorReporter.h"
#include "GHHMDRenderDevice.h"
#include "GHGLESStateCache.h"
#include "GHRenderTarget.h"

GHGLESRenderDevice::GHGLESRenderDevice(GHGLESContext& context, GHMutex& renderMutex, 
	GHGLESStateCache& stateCache, const int& graphicsQuality)
: mContext(context)
, mStateCache(stateCache)
, mRenderMutex(renderMutex)
, mGraphicsQuality(graphicsQuality)
{
	mStateCache.setGraphicsQualityReference(&mGraphicsQuality);

	glClearDepthf(1.0f);
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);	
    
	//const GLubyte* extensions = glGetString(GL_EXTENSIONS);
	//GHDebugMessage::outputString((const char*)extensions);

	GHGLESErrorReporter::checkAndReportError("construct GHGLESRenderDevice");
}

bool GHGLESRenderDevice::beginFrame(void)
{
    mRenderMutex.acquire();

	// need to reset the state cache between frames to avoid any potential badness.
	mStateCache.clearStates();

    mContext.beginFrame();
    
	if (mHMDDevice && mHMDDevice->isActive())
	{
		mHMDDevice->beginFrame();
	}

    return true;
}

void GHGLESRenderDevice::endFrame(void)
{
    mContext.endFrame();

	if (mHMDDevice && mHMDDevice->isActive())
	{
		mHMDDevice->endFrame();
	}

    mRenderMutex.release();
}

void GHGLESRenderDevice::createDeviceViewTransforms(const GHViewInfo::ViewTransforms& engineTransforms,
	GHViewInfo::ViewTransforms& deviceTransforms,
	const GHCamera& camera, bool isRenderToTexture) const
{
	deviceTransforms = engineTransforms;

	deviceTransforms.mPlatformGUITrans = GHTransform(
		2.0, 0.0, 0.0, 0.0,
		0.0, -2.0, 0.0, 0.0,
		0.0, 0.0, -1.0, 0.0,
		-1.0, 1.0, 0.0, 1.0
	);

	// if we are render to texture then we need to flip y in order to match directx.
	if (isRenderToTexture)
	{
		deviceTransforms.mPlatformGUITrans[5] *= -1;
		deviceTransforms.mProjectionTransform[5] *= -1;
		deviceTransforms.calcDerivedTransforms(camera);
	}
}

void GHGLESRenderDevice::beginRenderPass(GHRenderTarget* optionalTarget, const GHPoint4& clearColor, bool clearBuffers)
{
	mRenderTarget = optionalTarget;
	setClearColor(clearColor);
	if (mRenderTarget)
	{
		mRenderTarget->apply();
	}
	if (clearBuffers)
	{
		this->clearBuffers();
	}
}

void GHGLESRenderDevice::endRenderPass(void)
{
	if (mRenderTarget)
	{
		mRenderTarget->remove();
		mRenderTarget = 0;
	}
}

void GHGLESRenderDevice::setClearColor(const GHPoint4& color)
{
    glClearColor(color[0], color[1], color[2], color[3]);
	GHGLESErrorReporter::checkAndReportError("glClearColor");
}

void GHGLESRenderDevice::clearBuffers(void)
{
	GHGLESErrorReporter::checkAndReportError("pre-clearBuffers");
	glDepthMask(GL_TRUE);
	GHGLESErrorReporter::checkAndReportError("glDepthMask");
	glEnable(GL_DEPTH_TEST);
	GHGLESErrorReporter::checkAndReportError("glEnable(GL_DEPTH_TEST)");
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	GHGLESErrorReporter::checkAndReportError("clearBuffers");
}

void GHGLESRenderDevice::clearZBuffer(void)
{
	glDepthMask(GL_TRUE);
	glEnable(GL_DEPTH_TEST);
	glClear(GL_DEPTH_BUFFER_BIT);
	GHGLESErrorReporter::checkAndReportError("clearZBuffer");
}

void GHGLESRenderDevice::clearBackBuffer(void)
{
	glClear(GL_COLOR_BUFFER_BIT);
	GHGLESErrorReporter::checkAndReportError("clearBackBuffer");
}

void GHGLESRenderDevice::applyViewInfo(const GHViewInfo& viewInfo)
{
    mActiveViewInfo = viewInfo;
}

void GHGLESRenderDevice::handleGraphicsQualityChange(void)
{ 
	// trigger a partial reinit to get new shaders and new shader handles
	mContext.handleGraphicsQualityChanged();
}
