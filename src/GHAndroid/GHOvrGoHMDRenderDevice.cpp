#ifdef GH_OVR_GO

#include "GHOvrGoHMDRenderDevice.h"
#include "GHViewInfo.h"
#include "GHFrustum.h"
#include "GHCamera.h"
#include "GHGLESErrorReporter.h"
#include <stdlib.h>
#include "GHPlatform/GHDebugMessage.h"
#include "GHOvrGoRenderTarget.h"
#include "GHRenderServices.h"
#include "GHOvrGoFrameState.h"
#include "GHUtils/GHProfiler.h"
#include "GHString/GHString.h"
#include "GHPlatform/GHThreadFactory.h"
#include "GHPlatform/GHMutex.h"
#include "GHPlatform/GHThread.h"
#include "GHPlatform/GHThreadSignal.h"

// EXT_texture_border_clamp
#ifndef GL_CLAMP_TO_BORDER
#define GL_CLAMP_TO_BORDER			0x812D
#endif

#ifndef GL_TEXTURE_BORDER_COLOR
#define GL_TEXTURE_BORDER_COLOR		0x1004
#endif

static const int NUM_MULTI_SAMPLES = 4;

#if !defined( GL_EXT_multisampled_render_to_texture )
typedef void (GL_APIENTRY* PFNGLRENDERBUFFERSTORAGEMULTISAMPLEEXTPROC) (GLenum target, GLsizei samples, GLenum internalformat, GLsizei width, GLsizei height);
typedef void (GL_APIENTRY* PFNGLFRAMEBUFFERTEXTURE2DMULTISAMPLEEXTPROC) (GLenum target, GLenum attachment, GLenum textarget, GLuint texture, GLint level, GLsizei samples);
#endif

// used to toggle between submitting in main thread or submitting in a separate thread.
//#define SUBMIT_IN_SEPARATE_THREAD 1

GHOvrGoHMDRenderDevice::GHOvrGoHMDRenderDevice(GHRenderServices& renderServices, const GHThreadFactory& threadFactory,
												const ovrJava& ovrJavaDesc, const GHOvrGoFrameState& frameState)
	: mOvrJavaDesc(ovrJavaDesc)
	, mRenderServices(renderServices)
	, mFrameState(frameState)
	, mThreadFactory(threadFactory)
{
	for (size_t i = 0; i < 2; ++i)
	{
		mRenderTargets[i] = 0;
	}

	mSubmitFrameToOvrMutex = threadFactory.createMutex();

#ifndef SUBMIT_IN_SEPARATE_THREAD
	mSubmitSignal = threadFactory.createSignal();
#endif
}

GHOvrGoHMDRenderDevice::~GHOvrGoHMDRenderDevice(void)
{
	destroySwapChain();
	for (size_t i = 0; i < 2; ++i)
	{
		delete mRenderTargets[i];
	}

	destroySubmitRunnable();
	delete mSubmitFrameToOvrMutex;
	delete mSubmitSignal;
}

void GHOvrGoHMDRenderDevice::beginFrame(void)
{
	if (!mOvrMobile) {
		return;
	}
}

void GHOvrGoHMDRenderDevice::endFrame(void)
{
	if (!mOvrMobile) {
		return;
	}

	// wait for the previous frame's submit to finish.
	mSubmitFrameToOvrMutex->acquire();
	mSubmitFrameToOvrMutex->release();

	GHGLESErrorReporter::checkAndReportError("pre-ovrgodevice endframe");

	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
	GHGLESErrorReporter::checkAndReportError("ovrgodevice endframe - bind null framebuffer");

	// Grab the info we need for submit while still on the main thread.
	// This line sets up the info for the next call to submitFrameToOvr()
	prepareStructuresForSubmitFrameToOvr();

#ifdef SUBMIT_IN_SEPARATE_THREAD

	// if we have not created a submit runnable yet do it now.
	// No longer creating a submit runnable as of the update to Ovr Mobile SDK 1.20.0. (Re: removal of completion fence sync objects)
	// This is probably a performance hit, we may want to revisit how we address this.
	// Relevant patch note from 1.20.0:
	//		Apps that previously supplied their own fence primitives will need to make sure that they follow the requirements of apps that do not supply
	//		completion fences. Specifically, they must call vrapi_SubmitFrame from a thread with a context bound so that synchronization primitives can be 
	//		inserted into the command queue. Apps that do their rendering on different threads / contexts must ensure that commands inserted into the 
	//		vrapi_SubmitFrame context command queue  do not complete before rendering until those other contexts have completed.
	// Relevant text from old API documentation:
	//		If, however, the application does not explicitly pass in sync objects, then vrapi_SubmitFrame()must be called from the thread with the OpenGL ES 
	//		context that was used for rendering, which allows vrapi_SubmitFrame() to add a sync object to the current context and check if rendering has completed.
	// TODO: Experiment with passing the context/surface to the submit thread using eglMakeCurrent.
	if (!mSubmitRunnable)
	{
		// don't create the submit runnable until we have data to send to it.
		mSubmitRunnable = new GHOvrGoHMDRenderDevice::SubmitToOvrRunnable(*this, *mSubmitFrameToOvrMutex, *mSubmitSignal);
		mSubmitThread = mThreadFactory.createThread(GHThread::TP_DEFAULT);
		mSubmitThread->runThread(*mSubmitRunnable);
	}

	// release the submit runnable for one frame.
	// this will trigger a submitFrameToOvr()
	mSubmitSignal->signal();

#else

	// if we don't have a submit runnable then just do it inline here.
	submitFrameToOvr();

#endif

	GHGLESErrorReporter::checkAndReportError("ovrgodevice endframe");
}

void GHOvrGoHMDRenderDevice::createStereoView(const GHViewInfo& mainView, const GHCamera& camera,
	GHViewInfo& leftView, GHViewInfo& rightView, GHFrustum& combinedFrustum) const
{
	leftView = mainView;
	rightView = mainView;

	{
		// pretty much a hack that works.
		// create a 90 degree fov frustum for culling.  75 is too small.  90 seems to work.
		// note: it's possible that we're really passing in fov/2 and are getting a 180 degree fov.
		// note: it's possible that we need to ask the oculus sdk for the real fov.
		GHTransform combinedProjection;
		GHViewInfo::calcProjectionTransform(camera.getNearClip(), camera.getFarClip(), GHPoint2(90.0f, 90.0f), combinedProjection);
		GHTransform combinedViewProj;
		mainView.getEngineTransforms().mViewTransform.mult(combinedProjection, combinedViewProj);
		combinedFrustum.createPlanes(combinedViewProj.getMatrix(), camera.getFarClip());
	}

	if (!mOvrMobile) {
		return;
	}

	GHTransform camTrans = camera.getTransform();

	GHViewInfo* viewInfoArray[2];
	viewInfoArray[0] = &leftView;
	viewInfoArray[1] = &rightView;
	for (int i = 0; i < VRAPI_FRAME_LAYER_EYE_MAX; ++i)
	{
		GHViewInfo::ViewTransforms& engineTransforms = viewInfoArray[i]->getEngineTransformsForModification();

		if (mFrameState.mIsTrackingReady)
		{
			ovrMatrix4f ovrproj = mFrameState.mPredictedTracking.Eye[i].ProjectionMatrix;
			for (int col = 0; col < 4; ++col)
			{
				for (int row = 0; row < 4; ++row)
				{
					int idx = row * 4 + col;
					engineTransforms.mProjectionTransform.getMatrix()[idx] = ovrproj.M[col][row];
				}
			}
		}

		viewInfoArray[i]->calcViewTransform(camTrans, engineTransforms.mViewTransform);
	}
}

bool GHOvrGoHMDRenderDevice::isActive(void) const
{
	return (mOvrMobile != 0);
}

GHRenderTarget* GHOvrGoHMDRenderDevice::getRenderTarget(RenderTargetType targetType) const
{
	if (targetType == GHHMDRenderDevice::RTT_LEFT)
	{
		return mRenderTargets[0];
	}
	if (targetType == GHHMDRenderDevice::RTT_RIGHT)
	{
		return mRenderTargets[1];
	}
	return 0;
}

void GHOvrGoHMDRenderDevice::setOvr(ovrMobile* ovr)
{ 
	if (mOvrMobile == ovr)
	{
		// nothing to do.
		return;
	}

	if (mOvrMobile)
	{
		destroySwapChain();
	}
	mOvrMobile = ovr; 
	if (mOvrMobile)
	{
		createSwapChain();
	}
	else
	{
		destroySubmitRunnable();
		mNumFramesSinceDevice = 0;
	}
}

void GHOvrGoHMDRenderDevice::createSwapChain(void)
{
	GHGLESErrorReporter::checkAndReportError("pre-createSwapChain");

	int eyeWidth = vrapi_GetSystemPropertyInt(&mOvrJavaDesc, VRAPI_SYS_PROP_SUGGESTED_EYE_TEXTURE_WIDTH);
	int eyeHeight = vrapi_GetSystemPropertyInt(&mOvrJavaDesc, VRAPI_SYS_PROP_SUGGESTED_EYE_TEXTURE_HEIGHT);
	eyeWidth *= 1.25;
	eyeHeight *= 1.25;
	GHDebugMessage::outputString("Using eye target size %d %d", eyeWidth, eyeHeight);
	
	const char* allExtensions = (const char*)glGetString(GL_EXTENSIONS);
	bool EXT_texture_border_clamp = strstr(allExtensions, "GL_EXT_texture_border_clamp") ||
		strstr(allExtensions, "GL_OES_texture_border_clamp");
	PFNGLRENDERBUFFERSTORAGEMULTISAMPLEEXTPROC glRenderbufferStorageMultisampleEXT =
		(PFNGLRENDERBUFFERSTORAGEMULTISAMPLEEXTPROC)eglGetProcAddress("glRenderbufferStorageMultisampleEXT");
	PFNGLFRAMEBUFFERTEXTURE2DMULTISAMPLEEXTPROC glFramebufferTexture2DMultisampleEXT =
		(PFNGLFRAMEBUFFERTEXTURE2DMULTISAMPLEEXTPROC)eglGetProcAddress("glFramebufferTexture2DMultisampleEXT");

	for (int eye = 0; eye < VRAPI_FRAME_LAYER_EYE_MAX; ++eye)
	{
		mEyeSwapChains[eye].mColorTextureSwapChain = vrapi_CreateTextureSwapChain(VRAPI_TEXTURE_TYPE_2D, VRAPI_TEXTURE_FORMAT_8888,
			eyeWidth, eyeHeight, 1, true);
		mEyeSwapChains[eye].mTextureSwapChainLength = vrapi_GetTextureSwapChainLength(mEyeSwapChains[eye].mColorTextureSwapChain);
		mEyeSwapChains[eye].mDepthBuffers = (GLuint *)malloc(mEyeSwapChains[eye].mTextureSwapChainLength * sizeof(GLuint));
		mEyeSwapChains[eye].mFrameBuffers = (GLuint *)malloc(mEyeSwapChains[eye].mTextureSwapChainLength * sizeof(GLuint));

		GHGLESErrorReporter::checkAndReportError("pre-swapchain creation");
		for (int i = 0; i < mEyeSwapChains[eye].mTextureSwapChainLength; i++)
		{
			// Create the color buffer texture.
			const GLuint colorTexture = vrapi_GetTextureSwapChainHandle(mEyeSwapChains[eye].mColorTextureSwapChain, i);
			//GLenum colorTextureTarget = frameBuffer->UseMultiview ? GL_TEXTURE_2D_ARRAY : GL_TEXTURE_2D;
			GLenum colorTextureTarget = GL_TEXTURE_2D;
			glBindTexture(colorTextureTarget, colorTexture);
			if (EXT_texture_border_clamp)
			{
				glTexParameteri(colorTextureTarget, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
				glTexParameteri(colorTextureTarget, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
				GLfloat borderColor[] = { 0.0f, 0.0f, 0.0f, 0.0f };
				glTexParameterfv(colorTextureTarget, GL_TEXTURE_BORDER_COLOR, borderColor);
			}
			else
			{
				// Just clamp to edge. However, this requires manually clearing the border
				// around the layer to clear the edge texels.
				glTexParameteri(colorTextureTarget, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
				glTexParameteri(colorTextureTarget, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
			}
			glTexParameteri(colorTextureTarget, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
			glTexParameteri(colorTextureTarget, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
			glBindTexture(colorTextureTarget, 0);
			GHGLESErrorReporter::checkAndReportError("swapchain color texture creation");

			// todo? multiview? probably not.

			// create the depth and render targets.
			if (glRenderbufferStorageMultisampleEXT != NULL && glFramebufferTexture2DMultisampleEXT != NULL)
			{
				mEyeSwapChains[eye].mMultiSamples = NUM_MULTI_SAMPLES;

				// Create multisampled depth buffer.
				glGenRenderbuffers(1, &mEyeSwapChains[eye].mDepthBuffers[i]);
				glBindRenderbuffer(GL_RENDERBUFFER, mEyeSwapChains[eye].mDepthBuffers[i]);
				glRenderbufferStorageMultisampleEXT(GL_RENDERBUFFER, mEyeSwapChains[eye].mMultiSamples, 
					GL_DEPTH_COMPONENT24, eyeWidth, eyeHeight);
				glBindRenderbuffer(GL_RENDERBUFFER, 0);

				// Create the frame buffer.
				// NOTE: glFramebufferTexture2DMultisampleEXT only works with GL_FRAMEBUFFER.
				glGenFramebuffers(1, &mEyeSwapChains[eye].mFrameBuffers[i]);
				glBindFramebuffer(GL_FRAMEBUFFER, mEyeSwapChains[eye].mFrameBuffers[i]);
				glFramebufferTexture2DMultisampleEXT(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, 
					colorTexture, 0, mEyeSwapChains[eye].mMultiSamples);
				glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, mEyeSwapChains[eye].mDepthBuffers[i]);
				GLenum renderFramebufferStatus = glCheckFramebufferStatus(GL_FRAMEBUFFER);
				glBindFramebuffer(GL_FRAMEBUFFER, 0);

				GHGLESErrorReporter::checkAndReportError("swapchain multisample buffer creation");
				if (renderFramebufferStatus != GL_FRAMEBUFFER_COMPLETE)
				{
					GHDebugMessage::outputString("Incomplete frame buffer object: %d", renderFramebufferStatus);
					return;
				}
			}
			else
			{
				mEyeSwapChains[eye].mMultiSamples = 1;

				// Create depth buffer.
				glGenRenderbuffers(1, &mEyeSwapChains[eye].mDepthBuffers[i]);
				glBindRenderbuffer(GL_RENDERBUFFER, mEyeSwapChains[eye].mDepthBuffers[i]);
				glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, eyeWidth, eyeHeight);
				glBindRenderbuffer(GL_RENDERBUFFER, 0);

				// Create the frame buffer.
				glGenFramebuffers(1, &mEyeSwapChains[eye].mFrameBuffers[i]);
				glBindFramebuffer(GL_DRAW_FRAMEBUFFER, mEyeSwapChains[eye].mFrameBuffers[i]);
				glFramebufferRenderbuffer(GL_DRAW_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, mEyeSwapChains[eye].mDepthBuffers[i]);
				glFramebufferTexture2D(GL_DRAW_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, colorTexture, 0);
				GLenum renderFramebufferStatus = glCheckFramebufferStatus(GL_DRAW_FRAMEBUFFER);
				glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);

				GHGLESErrorReporter::checkAndReportError("swapchain single sample buffer creation");

				if (renderFramebufferStatus != GL_FRAMEBUFFER_COMPLETE)
				{
					GHDebugMessage::outputString("Incomplete frame buffer object: %d", renderFramebufferStatus);
					return;
				}
			}
		}
	}

	for (int i = 0; i < 2; ++i)
	{
		delete mRenderTargets[i];
		mRenderTargets[i] = new GHOvrGoRenderTarget(*mRenderServices.getDevice(), eyeWidth, eyeHeight, mEyeSwapChains[i].mFrameBuffers, mEyeSwapChains[i].mTextureSwapChainIndex);
	}

	GHGLESErrorReporter::checkAndReportError("createSwapChain");
}

void GHOvrGoHMDRenderDevice::destroySwapChain(void)
{
	GHGLESErrorReporter::checkAndReportError("pre destroy swap chain");

	for (int eye = 0; eye < VRAPI_FRAME_LAYER_EYE_MAX; ++eye)
	{
		glDeleteFramebuffers(mEyeSwapChains[eye].mTextureSwapChainLength, mEyeSwapChains[eye].mFrameBuffers);
		glDeleteRenderbuffers(mEyeSwapChains[eye].mTextureSwapChainLength, mEyeSwapChains[eye].mDepthBuffers);
		vrapi_DestroyTextureSwapChain(mEyeSwapChains[eye].mColorTextureSwapChain);

		free(mEyeSwapChains[eye].mDepthBuffers);
		free(mEyeSwapChains[eye].mFrameBuffers);
	}

	for (int i = 0; i < 2; ++i)
	{
		delete mRenderTargets[i];
		mRenderTargets[i] = 0;
	}

	GHGLESErrorReporter::checkAndReportError("destroy swap chain");
}

void GHOvrGoHMDRenderDevice::prepareStructuresForSubmitFrameToOvr(void)
{
	if (!mOvrMobile)
	{
		return;
	}

	mPendingSubmitFrameDescription.mLayer = vrapi_DefaultLayerProjection2();
	mPendingSubmitFrameDescription.mLayer.HeadPose = mFrameState.mPredictedTracking.HeadPose;

	for (int eye = 0; eye < VRAPI_FRAME_LAYER_EYE_MAX; eye++)
	{
		mPendingSubmitFrameDescription.mLayer.Textures[eye].ColorSwapChain = mEyeSwapChains[eye].mColorTextureSwapChain;
		mPendingSubmitFrameDescription.mLayer.Textures[eye].SwapChainIndex = mEyeSwapChains[eye].mTextureSwapChainIndex;
		mPendingSubmitFrameDescription.mLayer.Textures[eye].TexCoordsFromTanAngles = ovrMatrix4f_TanAngleMatrixFromProjection(&mFrameState.mPredictedTracking.Eye[eye].ProjectionMatrix);
	}
	// turn off for speed?
	mPendingSubmitFrameDescription.mLayer.Header.Flags |= VRAPI_FRAME_LAYER_FLAG_CHROMATIC_ABERRATION_CORRECTION;

	mPendingSubmitFrameDescription.mLayerList[0] = &mPendingSubmitFrameDescription.mLayer.Header;
	int layerCount = 1;

	mPendingSubmitFrameDescription.mSubmitFrameDescription.Flags = 0;
	mPendingSubmitFrameDescription.mSubmitFrameDescription.SwapInterval = 1; // 1 for 60, 2 for 30.
	mPendingSubmitFrameDescription.mSubmitFrameDescription.FrameIndex = mFrameState.mFrameId;
	mPendingSubmitFrameDescription.mSubmitFrameDescription.DisplayTime = mFrameState.mPredictedDisplayTime;
	mPendingSubmitFrameDescription.mSubmitFrameDescription.LayerCount = layerCount;
	mPendingSubmitFrameDescription.mSubmitFrameDescription.Layers = mPendingSubmitFrameDescription.mLayerList;

    for (int eye = 0; eye < VRAPI_FRAME_LAYER_EYE_MAX; ++eye)
    {
        mEyeSwapChains[eye].mTextureSwapChainIndex++;
        if (mEyeSwapChains[eye].mTextureSwapChainIndex >= mEyeSwapChains[eye].mTextureSwapChainLength)
        {
            mEyeSwapChains[eye].mTextureSwapChainIndex = 0;
        }
    }
}

void GHOvrGoHMDRenderDevice::submitFrameToOvr(void)
{
	GHPROFILESCOPE("GHOvrGoHMDRenderDevice::submitFrameToOvr()", GHString::CHT_REFERENCE);

	if (!mOvrMobile)
	{
		return;
	}

	vrapi_SubmitFrame2(mOvrMobile, &mPendingSubmitFrameDescription.mSubmitFrameDescription);

	GHGLESErrorReporter::checkAndReportError("ovrgodevice endframe - submit to ovr");
}

void GHOvrGoHMDRenderDevice::destroySubmitRunnable(void)
{
	// mSubmitRunnable should only be non-null if SUBMIT_IN_SEPARATE_THREAD
#ifdef SUBMIT_IN_SEPARATE_THREAD 
	if (mSubmitRunnable)
	{
		mSubmitRunnable->flagExit();
		mSubmitSignal->signal();
		// wait for the signal to complete.
		mSubmitFrameToOvrMutex->acquire();
		delete mSubmitRunnable; 
		delete mSubmitThread;
		mSubmitRunnable = 0;
		mSubmitThread = 0;
		mSubmitFrameToOvrMutex->release();
	}
#endif
}

GHOvrGoHMDRenderDevice::SubmitToOvrRunnable::SubmitToOvrRunnable(GHOvrGoHMDRenderDevice& parent, GHMutex& submitMutex, GHThreadSignal& submitSignal)
	: mParent(parent)
	, mSubmitMutex(submitMutex)
	, mSignal(submitSignal)
{
}

void GHOvrGoHMDRenderDevice::SubmitToOvrRunnable::flagExit(void)
{ 
	mSubmitMutex.acquire();
	mExitFlag = true; 
	mSubmitMutex.release();
}

void GHOvrGoHMDRenderDevice::SubmitToOvrRunnable::run(void)
{
	while (true)
	{
		// wait for the calling thread to have info for us and be ready.
		mSubmitMutex.acquire();
		mSignal.wait(mSubmitMutex);

		if (mExitFlag)
		{
		    mSubmitMutex.release();
			return;
		}

		// do the expensive 3ms call.
		mParent.submitFrameToOvr();
		// tell parent that it is ok to start the next frame.
		mSubmitMutex.release();
	}
}

#endif
