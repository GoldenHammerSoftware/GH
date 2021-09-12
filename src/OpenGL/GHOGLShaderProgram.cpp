// Copyright 2010 Golden Hammer Software
#include "GHOGLShaderProgram.h"
#include "GHOGLInclude.h"

GHOGLShaderProgram::GHOGLShaderProgram(unsigned int program)
: mShaderProgram(program)
{
}

GHOGLShaderProgram::~GHOGLShaderProgram(void)
{
    glDeleteProgram(mShaderProgram);
}
