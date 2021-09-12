#include "GHGLESStateCache.h"
#include "GHGLESInclude.h"
#include "GHGLESErrorReporter.h"
#include <algorithm> // min
#include "GHGLESExtensions.h"
#include "GHPlatform/GHDebugMessage.h"

GHGLESStateCache::GHGLESStateCache(GHEventMgr& eventMgr)
	: mReinitListener(eventMgr, *this)
{
	clearStates();
}


void GHGLESStateCache::onDeviceReinit(void)
{
	clearStates();
}

void GHGLESStateCache::clearStates(void)
{
	mShaderValid = false;
	mAlphaEnableValid = false;
	mAlphaFuncValid = false;
	mZReadValid = false;
	mZWriteValid = false;
	mCullValid = false;
	mCullFaceValid = false;
	mZOffsetValid = false;

	clearTextureStates();
}

void GHGLESStateCache::clearTextureStates(void)
{
	mActiveTextureValid = false;
	for (int i = 0; i < kMaxTrackedTextures; ++i)
	{
		mTextureWrapValid[i] = false;
		mTextureHandlesValid[i] = false;
		mTextureMipValid[i] = false;
	}
}

void GHGLESStateCache::setShader(unsigned int shaderHandle)
{
	// filter out repeated shader calls.
	if (mShaderValid && shaderHandle == mLastShaderHandle) return;
	mLastShaderHandle = shaderHandle;
	glUseProgram(shaderHandle);
	GHGLESErrorReporter::checkAndReportError("glUseProgram");
	mShaderValid = true;

	// we might need to set some other flags as invalid when the shader changes.
	clearTextureStates();
}

void GHGLESStateCache::setAlphaEnable(bool enable)
{
	if (mAlphaEnableValid && enable == mAlphaEnable) return;
	mAlphaEnable = enable;
	mAlphaEnableValid = true;
	if (mAlphaEnable) glEnable(GL_BLEND);
	else glDisable(GL_BLEND);
}

void GHGLESStateCache::setAlphaFunc(GLenum srcBlend, GLenum dstBlend)
{
	if (mAlphaFuncValid && (srcBlend == mSrcBlend && dstBlend == mDstBlend)) return;
	mSrcBlend = srcBlend;
	mDstBlend = dstBlend;
	mAlphaFuncValid = true;
	glBlendFunc(mSrcBlend, mDstBlend);
}

void GHGLESStateCache::setZRead(bool enable)
{
	if (mZReadValid && enable == mZReadEnable) return;
	mZReadEnable = enable;
	mZReadValid = true;
	if (mZReadEnable) glEnable(GL_DEPTH_TEST);
	else glDisable(GL_DEPTH_TEST);
}

void GHGLESStateCache::setZWrite(bool enable)
{
	if (mZWriteValid && enable == mZWriteEnable) return;
	mZWriteEnable = enable;
	mZWriteValid = true;
	if (mZWriteEnable) glDepthMask(GL_TRUE);
	else glDepthMask(GL_FALSE);
}

void GHGLESStateCache::setZOffset(bool enable)
{
	if (mZOffsetValid && enable == mZOffset) return;
	mZOffset = enable;
	mZOffsetValid = true;
	if (mZOffset) glDepthRangef(0, 1);
	else glDepthRangef(0.1, 1);
}

void GHGLESStateCache::setCullEnabled(bool enable)
{
	if (mCullValid && enable == mCullEnable) return;
	mCullEnable = enable;
	mCullValid = true;
	if (mCullEnable) glEnable(GL_CULL_FACE);
	else glDisable(GL_CULL_FACE);
}

void GHGLESStateCache::setCullFace(GLenum face)
{
	if (mCullFaceValid && face == mCullFace) return;
	mCullFace = face;
	mCullFaceValid = true;
	glCullFace(face);
}

void GHGLESStateCache::setActiveTexture(unsigned int texIdx)
{
	if (mActiveTextureValid && texIdx == mActiveTextureIndex) return;
	mActiveTextureIndex = texIdx;
	mActiveTextureValid = true;
	int modifiedIdx = GL_TEXTURE0 + texIdx;
	glActiveTexture(modifiedIdx);
}

void GHGLESStateCache::setTextureWrap(bool wrap, GLint bindType)
{
	if (mActiveTextureIndex < kMaxTrackedTextures)
	{
		if (mTextureWrapValid[mActiveTextureIndex] && mTextureWrap[mActiveTextureIndex] == wrap) return;
		mTextureWrap[mActiveTextureIndex] = wrap;
		mTextureWrapValid[mActiveTextureIndex] = true;
	}

	GLfloat wrapMode = GL_CLAMP_TO_EDGE;
	if (wrap)
	{
		wrapMode = GL_REPEAT;
	}

	glTexParameterf(bindType, GL_TEXTURE_WRAP_S, wrapMode);
	glTexParameterf(bindType, GL_TEXTURE_WRAP_T, wrapMode);
	if (bindType == GL_TEXTURE_CUBE_MAP)
	{
		glTexParameterf(bindType, GL_TEXTURE_WRAP_R, wrapMode);
	}
}

void GHGLESStateCache::setTextureMipmap(bool mipmap, GLint bindType)
{
	/*
	// This naieve implementation of caching mipmap does not seem to work, so it is disabled.
	// We end up not having min filter set somehow
	if (mActiveTextureIndex < kMaxTrackedTextures)
	{
		if (mTextureMipValid[mActiveTextureIndex] && mTextureMip[mActiveTextureIndex] == mipmap) return;
		mTextureMip[mActiveTextureIndex] = mipmap;
		mTextureMipValid[mActiveTextureIndex] = true;
	}
	*/

	if (mipmap)
	{
        if (!mCheckedDeviceAnistropy)
        {
            mCheckedDeviceAnistropy = true;
            
#define GH_ALLOW_ANISTROPY 1
#if GH_ALLOW_ANISTROPY
            mImplementationSupportsAnistropy = GHGLESExtensions::checkExtension("EXT_texture_filter_anisotropic");
            glGetFloatv(GL_MAX_TEXTURE_MAX_ANISOTROPY_EXT, &mMaxAnistropy);
            mMaxAnistropy = std::min(8.0f, mMaxAnistropy);
			if (!mImplementationSupportsAnistropy) GHDebugMessage::outputString("Not using anistropic filtering");
			else { GHDebugMessage::outputString("Using anistropy %f", mMaxAnistropy); }
#endif
        }
        
		// Todo: support other filtering modes.
		glTexParameterf(bindType, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(bindType, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);

        if (mImplementationSupportsAnistropy)
        {
			float anistropyToUse = mMaxAnistropy;
			if (mGraphicsQuality)
			{
				if (*mGraphicsQuality == 0)
				{
					anistropyToUse = 1.0f;
				}
				else if (*mGraphicsQuality < 2)
				{
					anistropyToUse = std::max(2.0f, mMaxAnistropy);
				}
			}
		    glTexParameterf(bindType, GL_TEXTURE_MAX_ANISOTROPY_EXT, anistropyToUse);
        }
	}
	else
	{
		glTexParameterf(bindType, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameterf(bindType, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	}
}

void GHGLESStateCache::setTextureHandle(unsigned int texIdx, int handle)
{
	if (texIdx < kMaxTrackedTextures)
	{
		if (mTextureHandlesValid[texIdx] && mTextureHandles[texIdx] == handle) return;
		mTextureHandlesValid[texIdx] = true;
		mTextureHandles[texIdx] = handle;
	}

	glUniform1i(handle, texIdx);
}
