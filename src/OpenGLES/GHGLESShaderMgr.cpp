// Copyright 2010 Golden Hammer Software
#include "GHGLESShaderMgr.h"
#include "GHGLESShaderProgram.h"
#include <algorithm>
#include "GHPlatform/GHDebugMessage.h"

GHGLESShaderMgr::GHGLESShaderMgr(void)
{
}

GHGLESShaderMgr::~GHGLESShaderMgr(void)
{
}

void GHGLESShaderMgr::registerShaderProgram(GHGLESShaderProgram* shaderProgram)
{
    mShaderPrograms.push_back(shaderProgram);
}

void GHGLESShaderMgr::unregisterShaderProgram(GHGLESShaderProgram* shaderProgram)
{
    ShaderProgramList::iterator iter = std::find(mShaderPrograms.begin(), mShaderPrograms.end(), shaderProgram);
    if (iter != mShaderPrograms.end())
    {
        mShaderPrograms.erase(iter);
    }
}

void GHGLESShaderMgr::reinitShaders(void)
{
    GHDebugMessage::outputString("reloading shaders");
    ShaderProgramList::iterator iter = mShaderPrograms.begin(), iterEnd = mShaderPrograms.end();
    for (; iter != iterEnd; ++iter)
    {
        GHGLESShaderProgram* program = *iter;
        program->reloadShaders();
    }
}

void GHGLESShaderMgr::handleGraphicsQualityChanged(void)
{
    GHDebugMessage::outputString("updating shaders for graphics settings");
    ShaderProgramList::iterator iter = mShaderPrograms.begin(), iterEnd = mShaderPrograms.end();
    for (; iter != iterEnd; ++iter)
    {
        GHGLESShaderProgram* program = *iter;
        program->handleGraphicsQualityChanged();
    }
}
