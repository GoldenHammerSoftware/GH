// Copyright 2010 Golden Hammer Software
#include "GHGLESRenderTarget.h"
#include "GHGLESInclude.h"
#include "GHGLESErrorReporter.h"
#include "GHGLESTexture.h"
#include "GHPlatform/GHDebugMessage.h"
#include "GHRenderProperties.h"

#define GHGLESRENDERTARGET_MSAA_COUNT 4

// 1 is the screen on iPhone for some reason
#ifdef ANDROID
	static const int sDEFAULT_TARGET = 0;
#else
	#ifdef BLACKBERRY
		static const int sDEFAULT_TARGET = 0;
	#else
		static const int sDEFAULT_TARGET = 1;
	#endif
#endif

GHGLESRenderTarget::GHGLESRenderTarget(GHGLESTexture* texture, GHEventMgr& eventMgr, const GHRenderTarget::Config& config)
: mDeviceListener(eventMgr, *this)
, mConfig(config)
{
	setTexture(texture, config);
}

GHGLESRenderTarget::~GHGLESRenderTarget(void)
{
	if (mTexture) mTexture->release();
}

void GHGLESRenderTarget::apply(void)
{
	GLint oldViewportParams[4];
	glGetIntegerv(GL_VIEWPORT, oldViewportParams);
    GHGLESErrorReporter::checkAndReportError("glGetIntegerv - GHGLESRenderTarget::apply");
	mReplacedViewportWidth = oldViewportParams[2];
	mReplacedViewportHeight = oldViewportParams[3];
	
	glBindFramebuffer(GL_FRAMEBUFFER, mFBO);
    GHGLESErrorReporter::checkAndReportError("glBindFrameBuffer - GHGLESRenderTarget::apply");
	// set the viewport as the FBO won't be the same dimension as the screen
	glViewport(0, 0, mConfig.mWidth, mConfig.mHeight);
	GHGLESErrorReporter::checkAndReportError("Render Target Apply");

    if(glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
        GHGLESErrorReporter::checkAndReportError("glCheckFrameBufferStatus - GHGLESRenderTarget::apply");
        static int first = 0;
        if (first++ == 0)
		GHDebugMessage::outputString("target framebuffer incomplete");
	}

	if (mConfig.mType == GHRenderTarget::RT_DEPTHONLY)
	{
		glColorMask(false, false, false, false);
	}
}

void GHGLESRenderTarget::remove(void)
{
	// if we're MSAA then resolve first.
	resolveMsaa();

	glBindFramebuffer(GL_FRAMEBUFFER, sDEFAULT_TARGET);
	glViewport(0, 0, mReplacedViewportWidth, mReplacedViewportHeight);
	GHGLESErrorReporter::checkAndReportError("Render Target Remove");

	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
		GHDebugMessage::outputString("base framebuffer incomplete");
	}

	if (usesMipmaps(mConfig))
	{
		mTexture->bind();
		glGenerateMipmap(GL_TEXTURE_2D);
		GHGLESErrorReporter::checkAndReportError("CreateOGLTexture glGenerateMipmap");
	}

	if (mConfig.mType == GHRenderTarget::RT_DEPTHONLY)
	{
		glColorMask(true, true, true, true);
	}
}

GHTexture* GHGLESRenderTarget::getTexture(void)
{
	return mTexture;
}

void GHGLESRenderTarget::initColorAndDepthFBO(unsigned int fbo, bool msaaTarget)
{
	if (msaaTarget)
	{
		// create a msaa target separate from our texture.
		GLuint rboColorId;
		glGenRenderbuffers(1, &rboColorId);
		glBindRenderbuffer(GL_RENDERBUFFER, rboColorId);
		glRenderbufferStorageMultisample(GL_RENDERBUFFER, GHGLESRENDERTARGET_MSAA_COUNT, GL_RGBA8, mConfig.mWidth, mConfig.mHeight);

		// attach colorbuffer image to FBO
		glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_RENDERBUFFER, rboColorId);
	}
	else
	{
		// associate texture with FBO
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D,
			mTexture->getTextureId(), 0);
	}

	// if we're the main target and not the resolve target then make a depth buffer.
	// we don't need a depth buffer for the resolve target.
	if (msaaTarget || !usesMsaa(mConfig))
	{
		unsigned int depthRenderBuffer = 0;

		glGenRenderbuffers(1, &depthRenderBuffer);
		GHGLESErrorReporter::checkAndReportError("glGenRenderbuffersOES");
		glBindRenderbuffer(GL_RENDERBUFFER, depthRenderBuffer);
		GHGLESErrorReporter::checkAndReportError("glBindRenderBufferOES-2");
		if (msaaTarget)
		{
			// create a multisample depth buffer.
			glRenderbufferStorageMultisample(GL_RENDERBUFFER, GHGLESRENDERTARGET_MSAA_COUNT, GL_DEPTH_COMPONENT16, mConfig.mWidth, mConfig.mHeight);
		}
		else
		{
			// create a depth buffer
			glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT16, mConfig.mWidth, mConfig.mHeight);
			GHGLESErrorReporter::checkAndReportError("glRenderbufferStorageOES");
		}

		// attach the generated depth buffer to the fbo.
		glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER,
			depthRenderBuffer);
		GHGLESErrorReporter::checkAndReportError("glFramebufferRenderbufferOES-2");
	}
}

void GHGLESRenderTarget::initDepthOnlyFBO(unsigned int fbo)
{
	// Instruct openGL that we won't bind a color texture with the currently bound FBO
	glReadBuffer(GL_NONE);

	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, mTexture->getTextureId(), 0);
}

bool GHGLESRenderTarget::createFBO(unsigned int& fbo, bool msaaTarget)
{
	bool ret = true;
	// generate FBO
	glGenFramebuffers(1, &fbo);
	glBindFramebuffer(GL_FRAMEBUFFER, fbo);

	if (mConfig.mType == GHRenderTarget::RT_DEPTHONLY)
	{
		assert(!msaaTarget);
		initDepthOnlyFBO(fbo);
	}
	else
	{
		initColorAndDepthFBO(fbo, msaaTarget);
	}

	GLenum fbStatus = glCheckFramebufferStatus(GL_FRAMEBUFFER);
	if (fbStatus != GL_FRAMEBUFFER_COMPLETE) {
		GHGLESErrorReporter::checkAndReportError("glCheckFrameBufferStatus - GHGLESRenderTarget::createOGLTarget");
		GHDebugMessage::outputString("Incomplete framebuffer!. fbStatus = 0x%x, msaaTarget %s", fbStatus, msaaTarget ? "yes" : "no");
		ret = false;
	}

	GHGLESErrorReporter::checkAndReportError("Render Target Creation");
	glBindFramebuffer(GL_FRAMEBUFFER, sDEFAULT_TARGET);
	GHGLESErrorReporter::checkAndReportError("glBindFrameBuffer");
	glBindRenderbuffer(GL_RENDERBUFFER, sDEFAULT_TARGET);

	return ret;
}

void GHGLESRenderTarget::createOGLTarget(void)
{
	if (usesMsaa(mConfig))
	{
		bool createdMsaa = createFBO(mFBO, true);
		if (!createdMsaa)
		{
			// some devices don't like render to texture msaa
			// the x86 samsung tab 3 is one of them.
			// maybe there's a better way to detect this ahead of time.
			mConfig.mMsaa = false;
			createdMsaa = createFBO(mFBO, false);
		}
		else
		{
			createFBO(mResolveFBO, false);
		}
	}
	else
	{
		createFBO(mFBO, false);
	}
}

void GHGLESRenderTarget::setTexture(GHGLESTexture* texture, const GHRenderTarget::Config& config)
{
    if (mTexture) mTexture->release();
	mConfig = config;
    mTexture = texture;
    mTexture->acquire();
	// asking the texture for dimensions in case mConfig is wrong?
	// seems shady.
    mTexture->getDimensions(mConfig.mWidth, mConfig.mHeight, mDepth);
	createOGLTarget();
}

void GHGLESRenderTarget::onDeviceReinit(void)
{
	createOGLTarget();
}

void GHGLESRenderTarget::resolveMsaa(void)
{
	if (!usesMsaa(mConfig))
	{
		return;
	}
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glBindFramebuffer(GL_READ_FRAMEBUFFER, mFBO);
	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, mResolveFBO);
	GHGLESErrorReporter::checkAndReportError("Render Target bind resolve targets");

	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
		GHDebugMessage::outputString("resolve framebuffer incomplete");
	}

	glBlitFramebuffer(0, 0, mConfig.mWidth, mConfig.mHeight,
		0, 0, mConfig.mWidth, mConfig.mHeight,
		GL_COLOR_BUFFER_BIT,
		GL_LINEAR);
	GHGLESErrorReporter::checkAndReportError("Render Target MSAA resolve");
}

bool GHGLESRenderTarget::usesMipmaps(const GHRenderTarget::Config& config)
{
	if (!config.mMipmap)
	{
		return false;
	}
	if (config.mType == GHRenderTarget::RT_DEPTHONLY)
	{
		// todo? add depth-only mipmap
		return false;
	}
	return true;
}

bool GHGLESRenderTarget::usesMsaa(const GHRenderTarget::Config& config)
{
	if (!config.mMsaa)
	{
		return false;
	}
	if (config.mType == GHRenderTarget::RT_DEPTHONLY)
	{
		// todo? add depth-only msaa
		return false;
	}
	return true;
}
