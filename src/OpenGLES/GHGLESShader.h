// Copyright 2010 Golden Hammer Software
#pragma once

#include "GHUtils/GHResource.h"
#include "GHString/GHString.h"
#include "GHUtils/GHMessageHandler.h"
#include "GHGLESInclude.h"
#include <map>

class GHGLESShaderLoader;
class GHEventMgr;

// A class to wrap a vert or pixel shader and manage calling glDeleteShader
// does not keep track of which programs reference it.
class GHGLESShader : public GHResource
{
public:
    GHGLESShader(unsigned int shaderId, GHGLESShaderLoader& loader, GHEventMgr& eventMgr, 
        const char* filename, GLenum type);
    unsigned int getId(void) const { return mId; }
    
    void reload(void);
    
    // add a version that should be used for a specific graphics quality.
    void addQualityVersion(int quality, GHGLESShader* version);
    // return a quality override if one exists, otherwise return this.
    GHGLESShader* getVersionForQuality(int quality);

    void setExpectsDXLayout(bool val) { mExpectsDXLayout = val; }
    bool getExpectsDXLayout(void) const { return mExpectsDXLayout; }

protected:
    ~GHGLESShader(void);
    
private:
    class DeviceListener : public GHMessageHandler
    {
    public:
        DeviceListener(GHEventMgr& eventMgr, unsigned int& idRef);
        ~DeviceListener(void);
        virtual void handleMessage(const GHMessage& message);
    private:
        GHEventMgr& mEventMgr;
        unsigned int& mIDRef;
    };
    
private:
    GHString mFilename;
    GLenum mType;
    GHGLESShaderLoader& mLoader;
    unsigned int mId;
    DeviceListener mDeviceListener;
    bool mExpectsDXLayout{ false };

    std::map<int, GHGLESShader*> mQualityVersions;
};
