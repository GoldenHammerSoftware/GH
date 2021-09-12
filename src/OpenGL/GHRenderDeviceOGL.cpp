// Copyright 2010 Golden Hammer Software
#include "GHRenderDeviceOGL.h"
#include "GHOGLContext.h"
#include "GHOGLINclude.h"
#include "GHDebugMessage.h"
#include "GHGLErrorReporter.h"
#include "GHViewInfo.h"
#include "GHMutex.h"

GHRenderDeviceOGL::GHRenderDeviceOGL(GHOGLContext& context, GHMutex& renderMutex)
: mContext(context)
, mRenderMutex(renderMutex)
{
	glShadeModel(GL_SMOOTH);
	glClearDepth(1.0f);
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);	
	glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);
    
	//const GLubyte* extensions = glGetString(GL_EXTENSIONS);
	//GHDebugMessage::outputString((const char*)extensions);
}

bool GHRenderDeviceOGL::beginFrame(void)
{
    mRenderMutex.acquire();
    mContext.beginFrame();
    
    return true;
}

void GHRenderDeviceOGL::endFrame(void)
{
    mContext.swapBuffers();
    mRenderMutex.release();
}

void GHRenderDeviceOGL::setClearColor(const GHPoint4& color)
{
    glClearColor(color[0], color[1], color[2], color[3]);
}

void GHRenderDeviceOGL::clearBuffers(void)
{
	glDepthMask(GL_TRUE);
	glEnable(GL_DEPTH_TEST);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	GHGLErrorReporter::checkAndReportError("clearBuffers");
}

void GHRenderDeviceOGL::clearZBuffer(void)
{
	glDepthMask(GL_TRUE);
	glEnable(GL_DEPTH_TEST);
	glClear(GL_DEPTH_BUFFER_BIT);
	GHGLErrorReporter::checkAndReportError("clearZBuffer");
}

void GHRenderDeviceOGL::clearBackBuffer(void)
{
	glClear(GL_COLOR_BUFFER_BIT);
}

void GHRenderDeviceOGL::applyViewInfo(const GHViewInfo& viewInfo)
{
    mActiveViewInfo = viewInfo;
    
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
    glMultMatrixf(viewInfo.getDeviceTransforms().mViewTransform.getMatrix().getCoords());
    
    glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
    glMultMatrixf(viewInfo.getDeviceTransforms().mProjectionTransform.getMatrix().getCoords());
}

