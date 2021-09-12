// Copyright 2010 Golden Hammer Software
#include "GHOGLTextureMgr.h"
#include "GHDebugMessage.h"
#include "GHGLErrorReporter.h"

GHOGLTextureMgr::GHOGLTextureMgr(void)
{
	GLint maxTexturesSupported;
	glGetIntegerv(GL_MAX_COMBINED_TEXTURE_IMAGE_UNITS, &maxTexturesSupported);
	GHDebugMessage::outputString("Max textures: %d", maxTexturesSupported);
	
	glEnable(GL_TEXTURE_2D);
	GHGLErrorReporter::checkAndReportError("glEnable(GL_TEXTURE_2D)");
}

GHOGLTextureMgr::~GHOGLTextureMgr(void)
{
    for (int i = 0; i < mActiveTextures.size(); ++i) 
    {
        glDeleteTextures(1, &mActiveTextures[i]);
    }
	
	glDisable(GL_TEXTURE_2D);
}

GLuint GHOGLTextureMgr::getUnusedHandle(void)
{
    GLuint ret;
	glGenTextures(1, &ret);
	GHGLErrorReporter::checkAndReportError("glGenTextures");
    mActiveTextures.push_back(ret);
    return ret;
}

void GHOGLTextureMgr::returnHandleToPool(GLuint id)
{
    std::vector<GLuint>::iterator findIter;
    for (findIter = mActiveTextures.begin(); findIter != mActiveTextures.end(); ++findIter)
    {
        if (*findIter == id) {
            mActiveTextures.erase(findIter);
            break;
        }
    }
    glDeleteTextures(1, &id);
}

