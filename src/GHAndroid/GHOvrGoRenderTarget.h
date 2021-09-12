#pragma once

#ifdef GH_OVR_GO

#include "GHRenderTarget.h"
#include <EGL/egl.h>
#include <EGL/eglext.h>
#include <GLES3/gl3.h>
#include <GLES3/gl3ext.h>

class GHRenderDevice;

class GHOvrGoRenderTarget : public GHRenderTarget
{
public:
	GHOvrGoRenderTarget(GHRenderDevice& ghRenderDevice, int width, int height, 
		GLuint* frameBuffers, int& swapChainIndex);

	virtual void apply(void);
	virtual void remove(void);

	// todo?
	virtual GHTexture* getTexture(void) { return 0; }

private:
	GHRenderDevice& mGHRenderDevice;

	int mWidth;
	int mHeight;
	int mReplacedViewportWidth;
	int mReplacedViewportHeight;

	GLuint* mFrameBuffers;
	int& mSwapChainIndex;
};

#endif