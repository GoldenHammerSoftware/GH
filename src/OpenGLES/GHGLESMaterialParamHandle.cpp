// Copyright 2010 Golden Hammer Software
#include "GHGLESMaterialParamHandle.h"
#include "GHGLESInclude.h"
#include "GHGLESErrorReporter.h"
#include "GHTexture.h"
#include "GHPlatform/GHDebugMessage.h"
#include "GHUtils/GHEventMgr.h"
#include "GHRenderProperties.h"
#include "GHGLESShaderProgram.h"
#include "GHGLESMaterialSH.h"
#include "GHGLESIdentifiers.h"

GHGLESMaterialParamHandle::GHGLESMaterialParamHandle(GHEventMgr& eventMgr, GHGLESShaderProgram* shaderProgram, 
    const char* paramName, int shaderHandle, int count, GHGLESMaterialSH& mat)
: mShaderHandle(shaderHandle)
, mDeviceListener(eventMgr, shaderProgram, paramName, mShaderHandle)
, mMat(mat)
, mCount(count)
{
}

void GHGLESMaterialParamHandle::setValue(HandleType type, const void* value)
{
    // -1 is ogl for not found. -2 is a gh code for found but not currently valid.
    if (mShaderHandle < 0)
    {
        // not found.
        return;
    }

    bool usesDXLayout = mMat.getExpectsDXLayout();
    GLboolean transpose = usesDXLayout? GL_TRUE : GL_FALSE;

	GHGLESErrorReporter::checkAndReportError("prior to GHGLESMaterialParamHandle::setValue");
	if (type == GHMaterialParamHandle::HT_TEXTURE)
	{
		GHMaterialParamHandle::TextureHandle* texHandle = (GHMaterialParamHandle::TextureHandle*)value;
        
        int textureOffset = mMat.getTextureIndex(mShaderHandle);
		int texIdx = GL_TEXTURE0+textureOffset;
		glUniform1i(mShaderHandle, textureOffset);
		GHGLESErrorReporter::checkAndReportError("callback - glUniform1i");
		glActiveTexture(texIdx);
		GHGLESErrorReporter::checkAndReportError("callback - glActiveTexture");
        
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
		GHGLESErrorReporter::checkAndReportError("callback - wrapmode");
        
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        GHGLESErrorReporter::checkAndReportError("callback - min/mag filter");

		texHandle->mTexture->bind();
		GHGLESErrorReporter::checkAndReportError("callback - texture bind");
	}
	else if (type == GHMaterialParamHandle::HT_MAT16)
    {
        glUniformMatrix4fv(mShaderHandle, mCount, transpose, (GLfloat*)value );
        GHGLESErrorReporter::checkAndReportError("callback - HT_MAT16");
    }
    else if (type == GHMaterialParamHandle::HT_MAT16x64)
    {
        glUniformMatrix4fv(mShaderHandle, 64, transpose, (GLfloat*)value );
		GHGLESErrorReporter::checkAndReportError("callback - HT_MAT16x64");
    }
    else if (type == GHMaterialParamHandle::HT_VEC4)
    {
        glUniform4fv(mShaderHandle, mCount, (GLfloat*)value);
    }
    else if (type == GHMaterialParamHandle::HT_VEC3)
    {
        glUniform3fv(mShaderHandle, mCount, (GLfloat*)value);
    }
    else if (type == GHMaterialParamHandle::HT_VEC2)
    {
        glUniform2fv(mShaderHandle, mCount, (GLfloat*)value);
    }
    else if (type == GHMaterialParamHandle::HT_FLOAT)
    {
        glUniform1fv(mShaderHandle, mCount, (GLfloat*)value);
    }
	else if (type == GHMaterialParamHandle::HT_INT)
	{
		glUniform1iv(mShaderHandle, mCount, (GLint*)value);
	}
    else 
    {
        GHDebugMessage::outputString("Unsupported shader argument.");
    }
    if (!GHGLESErrorReporter::checkAndReportError("After GHGLESMaterialParamHandle::setValue"))
    {
        GHDebugMessage::outputString("Shader param error mShaderHandle=%d paramName=%s count=%d", mShaderHandle, mDeviceListener.getParamName().getChars(), mCount);
    }
}

GHGLESMaterialParamHandle::DeviceListener::DeviceListener(GHEventMgr& eventMgr, GHGLESShaderProgram* shaderProgram, const char* paramName, int& handleRef)
: mEventMgr(eventMgr)
, mShaderProgram(shaderProgram)
, mHandleReference(handleRef)
{
    mShaderProgram->acquire();
    mParamName.setConstChars(paramName, GHString::CHT_COPY);
    mEventMgr.registerListener(GHRenderProperties::DEVICEREINIT, *this);
    mEventMgr.registerListener(GHGLESIdentifiers::M_RELOADMATERIALPARAMS, *this);
}

GHGLESMaterialParamHandle::DeviceListener::~DeviceListener(void)
{
    mShaderProgram->release();
    mEventMgr.unregisterListener(GHRenderProperties::DEVICEREINIT, *this);
    mEventMgr.unregisterListener(GHGLESIdentifiers::M_RELOADMATERIALPARAMS, *this);
}

void GHGLESMaterialParamHandle::DeviceListener::handleMessage(const GHMessage& message)
{
    // we expect that the message comes from GHGLESReinitializer and the shaders have already been reloaded.
    mHandleReference = mShaderProgram->getUniformLocation(mParamName.getChars());
}

