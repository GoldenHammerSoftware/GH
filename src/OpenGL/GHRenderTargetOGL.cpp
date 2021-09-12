// Copyright 2010 Golden Hammer Software
#include "GHRenderTargetOGL.h"
#include "GHRenderTargetOGL.h"
#include "GHTextureOGL.h"
#include "GHGLErrorReporter.h"
#include "GHDebugMessage.h"
#include "GHOGLInclude.h"

static const int sDEFAULT_TARGET = 0;

GHRenderTargetOGL::GHRenderTargetOGL(GHTextureOGL* texture)
: mTexture(0)
, mFBO(0)
, mDepthBuffer(0)
{
    GHRefCounted::changePointer((GHRefCounted*&)mTexture, texture);
	createOGLTarget();
}

GHRenderTargetOGL::~GHRenderTargetOGL(void)
{
    GHRefCounted::changePointer((GHRefCounted*&)mTexture, 0);
    glDeleteRenderbuffersEXT(1, &mDepthBuffer);
    glDeleteFramebuffers(1, &mFBO);
}

void GHRenderTargetOGL::apply(void)
{
	GLint oldViewportParams[4];
	glGetIntegerv(GL_VIEWPORT, oldViewportParams);
	mReplacedViewportWidth = oldViewportParams[2];
	mReplacedViewportHeight = oldViewportParams[3];
	
	glBindFramebuffer(GL_FRAMEBUFFER, mFBO);
	// set the viewport as the FBO won't be the same dimension as the screen
	unsigned int width, height, depth;
	mTexture->getDimensions(width, height, depth);	
	glViewport(0, 0, width, height);
	GHGLErrorReporter::checkAndReportError("Render Target Apply");
}

void GHRenderTargetOGL::remove(void)
{
	glBindFramebuffer(GL_FRAMEBUFFER, sDEFAULT_TARGET);
	glViewport(0, 0, mReplacedViewportWidth, mReplacedViewportHeight);
	GHGLErrorReporter::checkAndReportError("Render Target Remove");
}

GHTexture* GHRenderTargetOGL::getTexture(void)
{
	return mTexture;
}

void GHRenderTargetOGL::createOGLTarget(void)
{
	mTexture->bind();
	// generate FBO
	glGenFramebuffers(1, &mFBO);
	glBindFramebuffer(GL_FRAMEBUFFER, mFBO);
	// associate texture with FBO
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, 
                           mTexture->getTextureId(), 0);
    
    // generate a depth buffer as well.
    glGenRenderbuffersEXT(1, &mDepthBuffer); 
    glBindRenderbufferEXT(GL_RENDERBUFFER_EXT, mDepthBuffer);
    unsigned int width, height, depth;
    mTexture->getDimensions(width, height, depth);
    glRenderbufferStorageEXT(GL_RENDERBUFFER_EXT, GL_DEPTH_COMPONENT, width, height); 
    glFramebufferRenderbufferEXT(GL_FRAMEBUFFER_EXT, GL_DEPTH_ATTACHMENT_EXT, GL_RENDERBUFFER_EXT, mDepthBuffer);
    
	// clear texture bind
	glBindTexture(GL_TEXTURE_2D,0);	
	glBindFramebuffer(GL_FRAMEBUFFER, sDEFAULT_TARGET);
    
	GHGLErrorReporter::checkAndReportError("Render Target Creation");
}
