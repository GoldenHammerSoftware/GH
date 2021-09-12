// Copyright 2010 Golden Hammer Software
#pragma once

// A class to wrap a handle to an OGL pixel/vertex shader pair
class GHOGLShaderProgram
{
public:
    GHOGLShaderProgram(unsigned int program);
    ~GHOGLShaderProgram(void);
    
    unsigned int getShaderProgram(void) const { return mShaderProgram; }

private:
    unsigned int mShaderProgram;
};
