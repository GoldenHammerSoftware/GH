// Copyright 2010 Golden Hammer Software
#include "GHCocoaOGLContext.h"
#include "GHDebugMessage.h"
#include "GHOGLInclude.h"
#include "GHMessageTypes.h"
#include "GHEventMgr.h"
#include "GHScreenInfo.h"

GHCocoaOGLContext::GHCocoaOGLContext(NSView* view, GHEventMgr& eventMgr, GHScreenInfo& screenInfo)
: mView(view)
, mEventMgr(eventMgr)
, mMessageHandler(*this)
, mScreenInfo(screenInfo)
{
    mEventMgr.registerListener(GHMessageTypes::WINDOWRESIZE, mMessageHandler);
    
    NSOpenGLPixelFormatAttribute attrFSAA[] =
    {
		NSOpenGLPFADoubleBuffer,
        NSOpenGLPFAMultisample,
		NSOpenGLPFAAccelerated,
		NSOpenGLPFAColorSize, 32,
		NSOpenGLPFADepthSize, 32,
		NSOpenGLPFASamples, 4,
		NSOpenGLPFASampleBuffers, 1,
		NSOpenGLPFANoRecovery,
		0 
	};
	
	bool isFSAA = true;
	mPixelFormat = [[NSOpenGLPixelFormat alloc] initWithAttributes:attrFSAA];
	if (!mPixelFormat) 
	{
		GHDebugMessage::outputString("Falling back to low quality renderer.");
		
		NSOpenGLPixelFormatAttribute attr[] =
		{
			NSOpenGLPFADoubleBuffer,
			NSOpenGLPFAAccelerated,
			NSOpenGLPFAColorSize, 32,
			NSOpenGLPFADepthSize, 16,
			0 
		};
		mPixelFormat = [[NSOpenGLPixelFormat alloc] initWithAttributes:attr];
		isFSAA = false;
	}
    else GHDebugMessage::outputString("Using high quality renderer.");
    
	mOGLContext = [[NSOpenGLContext alloc] initWithFormat:mPixelFormat shareContext:nil];
    
	[mOGLContext setView:mView];
    
	// enable vsync
	GLint swapInterval = 1;
	[mOGLContext setValues:&swapInterval forParameter:NSOpenGLCPSwapInterval];
	
	[mOGLContext makeCurrentContext];	
	
	if (isFSAA) 
	{
		glEnable(GL_MULTISAMPLE_ARB);
		glHint (GL_MULTISAMPLE_FILTER_HINT_NV, GL_NICEST);
	}
    
    updateViewPort();
}

GHCocoaOGLContext::~GHCocoaOGLContext(void)
{
    mEventMgr.unregisterListener(GHMessageTypes::WINDOWRESIZE, mMessageHandler);
}

void GHCocoaOGLContext::beginFrame(void)
{
	[mOGLContext makeCurrentContext];	
}

void GHCocoaOGLContext::swapBuffers(void)
{
	[mOGLContext flushBuffer];	
}

void GHCocoaOGLContext::setFullScreen(bool isFull)
{
    if (isFull)
    {
        NSDictionary* fullscreenOptions = [NSDictionary dictionaryWithObjectsAndKeys:
                                           [NSNumber numberWithBool:NO], NSFullScreenModeAllScreens,  
                                           nil];
        
        [mView enterFullScreenMode:[NSScreen mainScreen] withOptions:fullscreenOptions];
    }
    else 
    {
        [mView exitFullScreenModeWithOptions:nil];
        [[mView window] makeFirstResponder:mView];
    }
    
    updateViewPort();
}

void GHCocoaOGLContext::updateViewPort(void)
{
	[mOGLContext makeCurrentContext];	
    
	NSSize windowSize = [mView bounds].size;
    mScreenInfo.setSize(GHPoint2i(windowSize.width, windowSize.height));

	glViewport(0, 0, windowSize.width, windowSize.height);	
	
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(45.0f, (GLfloat)windowSize.width/windowSize.height, 0.1f, 100.0f);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
}

GHCocoaOGLContext::MessageHandler::MessageHandler(GHCocoaOGLContext& context)
: mContext(context)
{
}

void GHCocoaOGLContext::MessageHandler::handleMessage(const GHMessage& message)
{
    mContext.updateViewPort();
    
    [mContext.mOGLContext update];
}

