// Copyright 2010 Golden Hammer Software
#include "GHGLESShader.h"
#include "GHGLESInclude.h"
#include "GHUtils/GHEventMgr.h"
#include "GHRenderProperties.h"
#include "GHGLESShaderLoader.h"

GHGLESShader::GHGLESShader(unsigned int shaderId, GHGLESShaderLoader& loader, GHEventMgr& eventMgr, 
    const char* filename, GLenum type)
: mLoader(loader)
, mId(shaderId)
, mType(type)
, mDeviceListener(eventMgr, mId)
{
    mFilename.setConstChars(filename, GHString::CHT_COPY);
}

GHGLESShader::~GHGLESShader(void)
{
    glDeleteShader(mId);

    for (auto vIter = mQualityVersions.begin(); vIter != mQualityVersions.end(); ++vIter)
    {
        vIter->second->release();
    }
}

void GHGLESShader::reload(void)
{
    // We want to be able to trigger a reload without an unload.
    // DJW is not sure why this check was here.
    //if (mId == 0)
    {
        bool expectsDX;
        mId = mLoader.loadShader(mFilename.getChars(), mType, false, expectsDX);
    }

    for (auto vIter = mQualityVersions.begin(); vIter != mQualityVersions.end(); ++vIter)
    {
        vIter->second->reload();
    }
}

void GHGLESShader::addQualityVersion(int quality, GHGLESShader* version)
{
    auto findIter = mQualityVersions.find(quality);
    if (findIter != mQualityVersions.end())
    {
        findIter->second->release();
    }
    version->acquire();
    mQualityVersions[quality] = version;
}

GHGLESShader* GHGLESShader::getVersionForQuality(int quality)
{
    auto findIter = mQualityVersions.find(quality);
    if (findIter == mQualityVersions.end())
    {
        return this;
    }
    return findIter->second;
}

GHGLESShader::DeviceListener::DeviceListener(GHEventMgr& eventMgr, unsigned int& idRef)
: mEventMgr(eventMgr)
, mIDRef(idRef)
{
    mEventMgr.registerListener(GHRenderProperties::DEVICELOST, *this);
}

GHGLESShader::DeviceListener::~DeviceListener(void)
{
    mEventMgr.unregisterListener(GHRenderProperties::DEVICELOST, *this);
}

void GHGLESShader::DeviceListener::handleMessage(const GHMessage& message)
{
    mIDRef = 0;
}
