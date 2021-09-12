// Copyright 2010 Golden Hammer Software
#include "GHiOSGLESContext.h"
#include "GHGLESErrorReporter.h"
#include "GHPlatform/GHDebugMessage.h"
#include "GHGLESInclude.h"
#import <UIKit/UIKit.h>
#include "GHUtils/GHEventMgr.h"
#include "GHMessageTypes.h"
#include "GHUtils/GHProfiler.h"

GHiOSGLESContext::GHiOSGLESContext(CAEAGLLayer* aglLayer, EAGLContext* context, bool msAllowed,
                                   GHEventMgr& eventMgr)
: mAGLLayer(aglLayer)
, mContext(context)
, mViewFramebuffer(0)
, mViewRenderbuffer(0)
, mDepthRenderbuffer(0)
, mMSAAFramebuffer(0)
, mMSAARenderbuffer(0)
, mMSAADepthBuffer(0)
, mSupportsAntiAliasing(false)
, mWindowSizeListener(*this, eventMgr)
, mBuffersNeedRecreate(false)
{
	NSArray* versionComponents = [[UIDevice currentDevice].systemVersion componentsSeparatedByString:@"."];
	NSString* majorVersionString = (NSString*)[versionComponents objectAtIndex:0];
	int majorVersionNumber = [majorVersionString intValue];
	if (msAllowed && majorVersionNumber > 3) {
		mSupportsAntiAliasing = true;
	}
    initRenderBufferStorage();
}

GHiOSGLESContext::~GHiOSGLESContext(void)
{
    destroyRenderBufferStorage();
}

void GHiOSGLESContext::initRenderBufferStorage(void)
{
	if (mSupportsAntiAliasing)
	{
		// these have to be generated before the regular buffers so they take index 1.
		// otherwise the multipass rendering won't work correctly.
		glGenFramebuffers(1, &mMSAAFramebuffer); 
		glGenRenderbuffers(1, &mMSAARenderbuffer);   
	}
	
	glGenFramebuffers(1, &mViewFramebuffer);
	GHGLESErrorReporter::checkAndReportError("glGenFramebuffersOES");
	glGenRenderbuffers(1, &mViewRenderbuffer);
	GHGLESErrorReporter::checkAndReportError("glGenRenderbuffersOES");
	
	glBindFramebuffer(GL_FRAMEBUFFER, mViewFramebuffer);
	GHGLESErrorReporter::checkAndReportError("glBindFramebufferOES");
	glBindRenderbuffer(GL_RENDERBUFFER, mViewRenderbuffer);
	GHGLESErrorReporter::checkAndReportError("glBindRenderbufferOES");
	[mContext renderbufferStorage:GL_RENDERBUFFER fromDrawable:mAGLLayer];
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_RENDERBUFFER, 
							  mViewRenderbuffer);
	GHGLESErrorReporter::checkAndReportError("glFramebufferRenderbufferOES");
	
	glGetRenderbufferParameteriv(GL_RENDERBUFFER, GL_RENDERBUFFER_WIDTH, &mBackingWidth);
	glGetRenderbufferParameteriv(GL_RENDERBUFFER, GL_RENDERBUFFER_HEIGHT, &mBackingHeight);
	GHGLESErrorReporter::checkAndReportError("glGetRenderbufferParameterivOES");
    
	if (!mSupportsAntiAliasing)
	{
		glGenRenderbuffers(1, &mDepthRenderbuffer);
		GHGLESErrorReporter::checkAndReportError("glGenRenderbuffers");
		glBindRenderbuffer(GL_RENDERBUFFER, mDepthRenderbuffer);
		GHGLESErrorReporter::checkAndReportError("glBindRenderBuffer-2");
		glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT16, mBackingWidth, mBackingHeight);
		GHGLESErrorReporter::checkAndReportError("glRenderbufferStorageOES");
		glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, 
								  mDepthRenderbuffer);
		GHGLESErrorReporter::checkAndReportError("glFramebufferRenderbuffer-2");
	}
	
	if(glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
		GHDebugMessage::outputString("failed to make a framebuffer obj");
	}
	
	glBindFramebuffer(GL_FRAMEBUFFER, mViewFramebuffer);
	glBindRenderbuffer(GL_RENDERBUFFER, mViewRenderbuffer);
	GHGLESErrorReporter::checkAndReportError("glBindFramebufferOES");
    
	if (mSupportsAntiAliasing)
	{
		glBindFramebuffer(GL_FRAMEBUFFER, mMSAAFramebuffer); 
		glBindRenderbuffer(GL_RENDERBUFFER, mMSAARenderbuffer);   
		// Generate the msaaDepthBuffer. 
		const int numSamples = 4;
        
        //GLenum colorBufferFormat = GL_RGB565; // matches kEAGLColorFormatRGB565?
        GLenum colorBufferFormat = GL_RGBA8; // matches kEAGLColorFormatRGBA8?

#if GH_ES_VERSION < 3
		glRenderbufferStorageMultisampleAPPLE(GL_RENDERBUFFER, numSamples, colorBufferFormat, mBackingWidth, mBackingHeight);
#else
        glRenderbufferStorageMultisample(GL_RENDERBUFFER, numSamples, colorBufferFormat, mBackingWidth, mBackingHeight);
#endif
        
		glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_RENDERBUFFER, mMSAARenderbuffer); 
		glGenRenderbuffers(1, &mMSAADepthBuffer);   
		//Bind the msaa depth buffer. 
		glBindRenderbuffer(GL_RENDERBUFFER, mMSAADepthBuffer); 
#if GH_ES_VERSION < 3
		glRenderbufferStorageMultisampleAPPLE(GL_RENDERBUFFER, numSamples, GL_DEPTH_COMPONENT16, mBackingWidth, mBackingHeight);
#else
        glRenderbufferStorageMultisample(GL_RENDERBUFFER, numSamples, GL_DEPTH_COMPONENT16, mBackingWidth, mBackingHeight);
#endif
		glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, mMSAADepthBuffer);	
        
		GHGLESErrorReporter::checkAndReportError("anti-alias framebuffer storage");
	}
    
    if(glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
		GHDebugMessage::outputString("failed to make a framebuffer obj");
	}
}

void GHiOSGLESContext::destroyRenderBufferStorage(void)
{
	glDeleteFramebuffers(1, &mViewFramebuffer);
	mViewFramebuffer = 0;
	glDeleteRenderbuffers(1, &mViewRenderbuffer);
	mViewRenderbuffer = 0;
	if (mDepthRenderbuffer) 
	{
		glDeleteRenderbuffers(1, &mDepthRenderbuffer);
		mDepthRenderbuffer = 0;
	}
	
	if (mSupportsAntiAliasing)
	{
		glDeleteFramebuffers(1, &mMSAAFramebuffer);
		mMSAAFramebuffer = 0;
		glDeleteRenderbuffers(1, &mMSAARenderbuffer);
		mMSAARenderbuffer = 0;
		glDeleteRenderbuffers(1, &mMSAADepthBuffer);
		mMSAADepthBuffer = 0;
	}
	
	GHGLESErrorReporter::checkAndReportError("glDeleteRenderbuffer");
}

void GHiOSGLESContext::beginFrame(void)
{
    if (mBuffersNeedRecreate) {
        destroyRenderBufferStorage();
        initRenderBufferStorage();
        mBuffersNeedRecreate = false;
    }
    
	[EAGLContext setCurrentContext:mContext];
	glViewport(0, 0, mBackingWidth, mBackingHeight);
	GHGLESErrorReporter::checkAndReportError("glViewport");
    
	if (mSupportsAntiAliasing)
	{
		glBindFramebuffer(GL_FRAMEBUFFER, mMSAAFramebuffer); 
		glBindRenderbuffer(GL_RENDERBUFFER, mMSAARenderbuffer);   
	}
}

void GHiOSGLESContext::endFrame(void)
{
#if GH_ES_VERSION < 3
    if (mSupportsAntiAliasing)
	{
        GHPROFILEBEGIN("anti-aliasing")
		//Bind both MSAA and View FrameBuffers. 
		glBindFramebuffer(GL_READ_FRAMEBUFFER_APPLE, mMSAAFramebuffer); 
		glBindFramebuffer(GL_DRAW_FRAMEBUFFER_APPLE, mViewFramebuffer); 

		// Call a resolve to combine both buffers 
		glResolveMultisampleFramebufferAPPLE();   
        
        // Apple (and the khronos group) encourages you to discard depth 
		// render buffer contents whenever is possible 
		GLenum attachments[] = {GL_DEPTH_ATTACHMENT, GL_COLOR_ATTACHMENT0}; 
		glDiscardFramebufferEXT(GL_READ_FRAMEBUFFER_APPLE, 2, attachments);

		// Present final image to screen 
		glBindRenderbuffer(GL_RENDERBUFFER, mViewRenderbuffer); 
		GHGLESErrorReporter::checkAndReportError("MSAA Present");
        GHPROFILEEND("anti-aliasing")
	}
	else {
        // Apple (and the khronos group) encourages you to discard depth
		// render buffer contents whenever is possible 
		GLenum attachments[] = {GL_DEPTH_ATTACHMENT, GL_COLOR_ATTACHMENT0}; 
		glDiscardFramebufferEXT(GL_READ_FRAMEBUFFER_APPLE, 2, attachments);
    }
#else
    if (mSupportsAntiAliasing)
    {
        GHPROFILEBEGIN("anti-aliasing")
        GHGLESErrorReporter::checkAndReportError("MSAA Pre Present");
        //Bind both MSAA and View FrameBuffers.
        glBindFramebuffer(GL_READ_FRAMEBUFFER, mMSAAFramebuffer);
        glBindFramebuffer(GL_DRAW_FRAMEBUFFER, mViewFramebuffer);
        GHGLESErrorReporter::checkAndReportError("MSAA Present glBindFramebuffer");
        
        // Call a resolve to combine both buffers
        glBlitFramebuffer(0,0,mBackingWidth,mBackingHeight, 0,0,mBackingWidth,mBackingHeight, GL_COLOR_BUFFER_BIT, GL_NEAREST);
        //glResolveMultisampleFramebufferAPPLE();
        GHGLESErrorReporter::checkAndReportError("MSAA Present - glBlitFramebuffer");
        
        // Apple (and the khronos group) encourages you to discard depth
        // render buffer contents whenever is possible
        GLenum attachments[] = {GL_DEPTH_ATTACHMENT, GL_COLOR_ATTACHMENT0};
        glInvalidateFramebuffer(GL_READ_FRAMEBUFFER, 2, attachments);
        GHGLESErrorReporter::checkAndReportError("MSAA Present - glInvalidateFramebuffer");
        
        // Present final image to screen
        glBindRenderbuffer(GL_RENDERBUFFER, mViewRenderbuffer);
        GHGLESErrorReporter::checkAndReportError("MSAA Present");
        GHPROFILEEND("anti-aliasing")
    }
    else {
        // Apple (and the khronos group) encourages you to discard depth
        // render buffer contents whenever is possible
        GLenum attachments[] = {GL_DEPTH_ATTACHMENT, GL_COLOR_ATTACHMENT0};
        glInvalidateFramebuffer(GL_READ_FRAMEBUFFER, 2, attachments);
    }
#endif
    
    GHPROFILEBEGIN("context present")
	[mContext presentRenderbuffer:GL_RENDERBUFFER];
    GHPROFILEEND("context present")
}

void GHiOSGLESContext::handleWindowSizeChanged(void)
{
    mBuffersNeedRecreate = true;
}

GHiOSGLESContext::WindowSizeListener::WindowSizeListener(GHiOSGLESContext& parent, GHEventMgr& eventMgr)
: mEventMgr(eventMgr)
, mParent(parent)
{
    mEventMgr.registerListener(GHMessageTypes::WINDOWRESIZE, *this);
}

GHiOSGLESContext::WindowSizeListener::~WindowSizeListener(void)
{
    mEventMgr.unregisterListener(GHMessageTypes::WINDOWRESIZE, *this);
}

void GHiOSGLESContext::WindowSizeListener::handleMessage(const GHMessage& message)
{
    mParent.handleWindowSizeChanged();
}

