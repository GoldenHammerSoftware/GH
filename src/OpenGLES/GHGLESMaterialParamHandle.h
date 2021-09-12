// Copyright 2010 Golden Hammer Software
#pragma once

#include "GHMaterialParamHandle.h"
#include "GHString/GHString.h"
#include "GHUtils/GHMessageHandler.h"

class GHGLESShaderProgram;
class GHEventMgr;
class GHGLESMaterialSH;

class GHGLESMaterialParamHandle : public GHMaterialParamHandle
{
public:
    GHGLESMaterialParamHandle(GHEventMgr& eventMgr, GHGLESShaderProgram* shaderProgram,
                              const char* paramName, int shaderHandle, int count, GHGLESMaterialSH& mat);
    
    virtual void setValue(HandleType type, const void* value);
    
private:
    class DeviceListener : public GHMessageHandler
    {
    public:
        DeviceListener(GHEventMgr& eventMgr, GHGLESShaderProgram* shaderProgram, const char* paramName, int& handleRef);
        ~DeviceListener(void);
        virtual void handleMessage(const GHMessage& message);
        const GHString& getParamName(void) const { return mParamName; }
    private:
        GHEventMgr& mEventMgr;
        GHGLESShaderProgram* mShaderProgram;
        GHString mParamName;
        int& mHandleReference;
    };
    
private:
    int mShaderHandle{ -1 };
	int mCount{ 1 };
    DeviceListener mDeviceListener;
    GHGLESMaterialSH& mMat;
};

