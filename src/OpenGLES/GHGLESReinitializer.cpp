// Copyright 2010 Golden Hammer Software
#include "GHGLESReinitializer.h"
#include "GHGLESTextureMgr.h"
#include "GHGLESShaderMgr.h"
#include "GHUtils/GHEventMgr.h"
#include "GHRenderProperties.h"
#include "GHUtils/GHMessage.h"
#include "GHGLESIdentifiers.h"

GHGLESReinitializer::GHGLESReinitializer(GHGLESTextureMgr& textureMgr, GHGLESShaderMgr& shaderMgr, GHEventMgr& eventMgr)
: mTextureMgr(textureMgr)
, mShaderMgr(shaderMgr)
, mEventMgr(eventMgr)
{
}

void GHGLESReinitializer::handleDeviceLost(void)
{
    mEventMgr.handleMessage(GHMessage(GHRenderProperties::DEVICELOST));
}

void GHGLESReinitializer::handleDeviceReinit(void)
{
    mTextureMgr.reinitTextures();
    mShaderMgr.reinitShaders();
    mEventMgr.handleMessage(GHMessage(GHRenderProperties::DEVICEREINIT));
}

void GHGLESReinitializer::handleGraphicsQualityChanged(void)
{
    mShaderMgr.handleGraphicsQualityChanged();
    // we need to reload the parmas after the shaders and before anything that uses them.
    mEventMgr.handleMessage(GHMessage(GHGLESIdentifiers::M_RELOADMATERIALPARAMS));
    mEventMgr.handleMessage(GHMessage(GHRenderProperties::M_GRAPHICSQUALITYCHANGED));
}
