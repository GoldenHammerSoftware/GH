#ifdef GH_OVR_GO

#include "GHOvrGoRenderTarget.h"
#include "GHRenderDevice.h"
#include "GHPlatform/GHDebugMessage.h"
#include "GHGLESErrorReporter.h"

static const int sDEFAULT_TARGET = 0;

GHOvrGoRenderTarget::GHOvrGoRenderTarget(GHRenderDevice& ghRenderDevice, int width, int height, GLuint* frameBuffers,
	int& swapChainIndex)
	: mWidth(width)
	, mHeight(height)
	, mFrameBuffers(frameBuffers)
	, mSwapChainIndex(swapChainIndex)
	, mGHRenderDevice(ghRenderDevice)
{
}

static const char* getFrameBufferStatusString(GLenum status)
{
	if (status == GL_FRAMEBUFFER_COMPLETE) return "no error";
	if (status == GL_FRAMEBUFFER_UNDEFINED) return "undefined";
	if (status == GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT) return "incomplete attachment";
	if (status == GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT) return "incomplete missing attachment";
	//if (status == GL_FRAMEBUFFER_INCOMPLETE_DRAW_BUFFER) return "incomplete draw buffer";
	//if (status == GL_FRAMEBUFFER_INCOMPLETE_READ_BUFFER) return "incomplete read buffer";
	if (status == GL_FRAMEBUFFER_UNSUPPORTED) return "unsupported";
	if (status == GL_FRAMEBUFFER_INCOMPLETE_MULTISAMPLE) return "incomplete multisample";
	//if (status == GL_FRAMEBUFFER_INCOMPLETE_LAYER_TARGETS) return "incomplete layer targets";
	return "unknown";
}

void GHOvrGoRenderTarget::apply(void)
{
	GLint oldViewportParams[4];
	glGetIntegerv(GL_VIEWPORT, oldViewportParams);
	GHGLESErrorReporter::checkAndReportError("glGetIntegerv - GHOvrGoRenderTarget::apply");
	mReplacedViewportWidth = oldViewportParams[2];
	mReplacedViewportHeight = oldViewportParams[3];

	// bind framebuffer.
	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, mFrameBuffers[mSwapChainIndex]);
	//glBindFramebuffer(GL_FRAMEBUFFER, mFBO);
	GHGLESErrorReporter::checkAndReportError("glBindFrameBuffer - GHGLESRenderTarget::apply");
	
	// set the viewport as the FBO won't be the same dimension as the screen
	glViewport(0, 0, mWidth, mHeight);
	GHGLESErrorReporter::checkAndReportError("Render Target Apply");

	GLenum framebufferStatus = glCheckFramebufferStatus(GL_DRAW_FRAMEBUFFER);
	if (framebufferStatus != GL_FRAMEBUFFER_COMPLETE) 
	{
		GHGLESErrorReporter::checkAndReportError("glCheckFrameBufferStatus - GHGLESRenderTarget::apply");
		GHDebugMessage::outputString("target framebuffer incomplete.  frameIndex %d", mSwapChainIndex);
		GHDebugMessage::outputString("framebuffer status: %s", getFrameBufferStatusString(framebufferStatus));
	}
	mGHRenderDevice.setIsStereo(true);
}

void GHOvrGoRenderTarget::remove(void)
{
	// somewhat assuming that this won't be called twice in a frame.
	// bad stuff will happen if we re-apply this target after invalidating the depth.

	GHGLESErrorReporter::checkAndReportError("pre ovrgodevice endframe");

	// Discard the depth buffer, so the tiler won't need to write it back out to memory.
	const GLenum depthAttachment[1] = { GL_DEPTH_ATTACHMENT };
	glInvalidateFramebuffer(GL_DRAW_FRAMEBUFFER, 1, depthAttachment);
	GHGLESErrorReporter::checkAndReportError("ovrgodevice endframe - invalidate depth");

	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, sDEFAULT_TARGET);
	glViewport(0, 0, mReplacedViewportWidth, mReplacedViewportHeight);
	GHGLESErrorReporter::checkAndReportError("Render Target Remove");

	GLenum framebufferStatus = glCheckFramebufferStatus(GL_DRAW_FRAMEBUFFER);
	if (framebufferStatus != GL_FRAMEBUFFER_COMPLETE)
	{
		GHDebugMessage::outputString("base framebuffer incomplete");
		GHDebugMessage::outputString("framebuffer status: %s", getFrameBufferStatusString(framebufferStatus));
	}

	mGHRenderDevice.setIsStereo(false);
}

#endif
