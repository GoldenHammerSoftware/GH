#pragma once

#ifdef GH_OVR_GO

#include "GHHMDRenderDevice.h"
#include "VrApi.h"
#include "VrApi_Helpers.h"
#include "VrApi_SystemUtils.h"
#include <EGL/egl.h>
#include <EGL/eglext.h>
#include <GLES3/gl3.h>
#include <GLES3/gl3ext.h>
#include "GHPlatform/GHRunnable.h"

class GHRenderServices;
class GHOvrGoRenderTarget;
class GHOvrGoFrameState;
class GHThreadFactory;
class GHMutex;
class GHThread;
class GHThreadSignal;

class GHOvrGoHMDRenderDevice : public GHHMDRenderDevice
{
public:
	GHOvrGoHMDRenderDevice(GHRenderServices& renderServices, const GHThreadFactory& threadFactory, 
		const ovrJava& ovrJavaDesc, const GHOvrGoFrameState& frameState);
	~GHOvrGoHMDRenderDevice(void);

	virtual void beginFrame(void);
	virtual void endFrame(void);

	virtual void createStereoView(const GHViewInfo& mainView, const GHCamera& camera,
		GHViewInfo& leftView, GHViewInfo& rightView, GHFrustum& combinedFrustum) const;

	// are we activated and being worn, ie should the game render in stereo to us.
	virtual bool isActive(void) const;

	virtual GHRenderTarget* getRenderTarget(RenderTargetType targetType) const;

	void setOvr(ovrMobile* ovr);

private:
	void createSwapChain(void);
	void destroySwapChain(void);

	// get everything ready for the next call to submitFrameToOvr
	// expected to be called from the main thread.
	// populates mPendingSubmitFrameDescription
	void prepareStructuresForSubmitFrameToOvr(void);
	// call the actual vrapi_SubmitFrame2 from the worker thread.
	void submitFrameToOvr(void);

	void destroySubmitRunnable(void);

private:
	GHRenderServices& mRenderServices;
	const ovrJava& mOvrJavaDesc;
	const GHOvrGoFrameState& mFrameState;
	ovrMobile* mOvrMobile{ 0 };

	struct SwapChain
	{
		ovrTextureSwapChain* mColorTextureSwapChain{ 0 };
		GLuint* mDepthBuffers{ 0 };
		GLuint* mFrameBuffers{ 0 };
		int mTextureSwapChainLength{ 0 };
		int mTextureSwapChainIndex{ 0 };
		int mMultiSamples{ 1 };
	};
	SwapChain mEyeSwapChains[VRAPI_FRAME_LAYER_EYE_MAX];

	GHOvrGoRenderTarget* mRenderTargets[2]; // explicitly 2 and not eye_max because ghengine only expects 2

	struct PendingSubmitFrameDescription
	{
		ovrSubmitFrameDescription2 mSubmitFrameDescription;
		ovrLayerProjection2 mLayer;
		const ovrLayerHeader2* mLayerList[ovrMaxLayerCount];
	};
	PendingSubmitFrameDescription mPendingSubmitFrameDescription;

	const GHThreadFactory& mThreadFactory;
	GHMutex* mSubmitFrameToOvrMutex{ 0 };
	GHThreadSignal* mSubmitSignal{ 0 };

	// runnable for hiding the submitFrameToOvr in a thread to buy back 3ms/frame.
	class SubmitToOvrRunnable : public GHRunnable
	{
	public:
		SubmitToOvrRunnable(GHOvrGoHMDRenderDevice& parent, GHMutex& submitMutex, GHThreadSignal& signal);

		virtual void run(void);

		void flagExit(void);

	private:
		GHOvrGoHMDRenderDevice & mParent;
		GHMutex& mSubmitMutex;
		GHThreadSignal& mSignal;
		bool mExitFlag{ false };
	};
	friend class SubmitToOvrRunnable; // friend so it can call the private submitFrameToOvr
	SubmitToOvrRunnable* mSubmitRunnable{ 0 };
	GHThread* mSubmitThread{ 0 };

	// trying to avoid some potential badness on the first frames.
	int mNumFramesSinceDevice{ 0 };
};

#endif