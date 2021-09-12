// Copyright 2010 Golden Hammer Software
#pragma once

#include <list>

class GHGLESShaderProgram;

//A central place for shader programs to be registered. Its primary purpose
// is to reload the shaders all at once on device reinit, so that they are
// guaranteed to be reloaded before the material param handles.
class GHGLESShaderMgr 
{
public:
    GHGLESShaderMgr(void);
    virtual ~GHGLESShaderMgr(void);
    
    void registerShaderProgram(GHGLESShaderProgram* shaderProgram);
    void unregisterShaderProgram(GHGLESShaderProgram* shaderProgram);
    
    void reinitShaders(void);
    void handleGraphicsQualityChanged(void);
    
private:
    typedef std::list<GHGLESShaderProgram*> ShaderProgramList;
    ShaderProgramList mShaderPrograms;
};
