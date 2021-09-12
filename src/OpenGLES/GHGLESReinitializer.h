// Copyright 2010 Golden Hammer Software
#pragma once

class GHGLESTextureMgr;
class GHGLESShaderMgr;
class GHEventMgr;

//Centralized place for the different pieces of the GLES renderer
// to be reinited in explicit order in cases like device reset.
//Move things here as it becomes necessary for them to be reinited
// in explicit order (as opposed to relying on order in individual message listening)
class GHGLESReinitializer
{
public:
    GHGLESReinitializer(GHGLESTextureMgr& textureMgr, GHGLESShaderMgr& shaderMgr, GHEventMgr& eventMgr);
    
    void handleDeviceLost(void);
    void handleDeviceReinit(void);
    // reinit whatever is needed for a quality change.
    // this is probably a much smaller list than handleDeviceReinit.
    void handleGraphicsQualityChanged(void);

private:
    GHGLESTextureMgr& mTextureMgr;
    GHGLESShaderMgr& mShaderMgr;
    GHEventMgr& mEventMgr;
};
