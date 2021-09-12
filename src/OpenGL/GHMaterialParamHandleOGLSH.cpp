// Copyright 2010 Golden Hammer Software
#include "GHMaterialParamHandleOGLSH.h"
#include "GHDebugMessage.h"
#include "GHGLErrorReporter.h"
#include "GHOGLInclude.h"
#include "GHTexture.h"
#include "GHMaterialOGLSH.h"

GHMaterialParamHandleOGLSH::GHMaterialParamHandleOGLSH(int shaderHandle, GHMaterialOGLSH& mat)
: mShaderHandle(shaderHandle)
, mMat(mat)
{
}

void GHMaterialParamHandleOGLSH::setValue(HandleType type, const void* value)
{
	if (type == GHMaterialParamHandle::HT_TEXTURE)
	{
		GHMaterialParamHandle::TextureHandle* texHandle = (GHMaterialParamHandle::TextureHandle*)value;

        int textureOffset = mMat.getTextureIndex(mShaderHandle);
		int texIdx = GL_TEXTURE0+textureOffset;
		glUniform1i(mShaderHandle, textureOffset);
		GHGLErrorReporter::checkAndReportError("callback - glUniform1i");
		glActiveTexture(texIdx);
		GHGLErrorReporter::checkAndReportError("callback - glActiveTexture");
        
        if (texHandle->mWrapMode == GHMDesc::WM_WRAP)
        {
            glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT );
            glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT );
        }
        else
        {
            glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE );
            glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE );
        }

		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

		texHandle->mTexture->bind();
	}
	else if (type == GHMaterialParamHandle::HT_MAT16)
    {
        glUniformMatrix4fv(mShaderHandle, 1, GL_FALSE, (GLfloat*)value );
    }
    else if (type == GHMaterialParamHandle::HT_VEC4)
    {
        glUniform4fv(mShaderHandle, 1, (GLfloat*)value);
    }
    else if (type == GHMaterialParamHandle::HT_VEC3)
    {
        glUniform3fv(mShaderHandle, 1, (GLfloat*)value);
    }
    else if (type == GHMaterialParamHandle::HT_VEC2)
    {
        glUniform2fv(mShaderHandle, 1, (GLfloat*)value);
    }
    else if (type == GHMaterialParamHandle::HT_FLOAT)
    {
        glUniform1fv(mShaderHandle, 1, (GLfloat*)value);
    }
    else 
    {
        GHDebugMessage::outputString("Unsupported shader argument.");
    }
}
