// Copyright 2010 Golden Hammer Software
#include "GHGLESTextureMgr.h"
#include "GHGLESErrorReporter.h"
#include "GHPlatform/GHDebugMessage.h"
#include "GHGLESTexture.h"
#include <algorithm>

GHGLESTextureMgr::GHGLESTextureMgr(void)
{
}

GHGLESTextureMgr::~GHGLESTextureMgr(void)
{
    for (int i = 0; i < mActiveTextures.size(); ++i) 
    {
        glDeleteTextures(1, &mActiveTextures[i]);
    }
}

GLuint GHGLESTextureMgr::getUnusedHandle(void)
{
    GLuint ret;
	glGenTextures(1, &ret);
	GHGLESErrorReporter::checkAndReportError("glGenTextures");
    mActiveTextures.push_back(ret);
    return ret;
}

void GHGLESTextureMgr::returnHandleToPool(GLuint id)
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

void GHGLESTextureMgr::registerTexture(GHGLESTexture* texture)
{
    mRegisteredTextures.push_back(texture);
}

void GHGLESTextureMgr::unregisterTexture(GHGLESTexture* texture)
{
    RegisteredTextures::iterator iter = std::find(mRegisteredTextures.begin(), mRegisteredTextures.end(), texture);
    if (iter != mRegisteredTextures.end())
    {
        mRegisteredTextures.erase(iter);
    }
}

void GHGLESTextureMgr::reinitTextures(void)
{
    mActiveTextures.clear();
    RegisteredTextures::iterator iter = mRegisteredTextures.begin(), iterEnd = mRegisteredTextures.end();
    for (; iter != iterEnd; ++iter)
    {
        GHGLESTexture* tex = *iter;
        tex->onDeviceReinit();
    }
}

