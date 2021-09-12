// Copyright Golden Hammer Software
#pragma once
#include "GHRenderDeviceReinitListener.h"
#include "GHGLESInclude.h"

// A way to avoid redundant ogl state calls on platforms where that matters.
class GHGLESStateCache
{
public:
	GHGLESStateCache(GHEventMgr& eventMgr);

	void setGraphicsQualityReference(const int* quality) { mGraphicsQuality = quality; }

	// Some values will go bad if carried over from one frame to the next.
	// handleNewFrame gives us a chance to mark those as invalid.
	void clearStates(void);

	void setShader(unsigned int shaderHandle);
	void setAlphaEnable(bool enable);
	void setAlphaFunc(GLenum srcBlend, GLenum dstBlend);
	void setZRead(bool enable);
	void setZWrite(bool enable);
	void setCullEnabled(bool enable);
	void setCullFace(GLenum face);
	void setZOffset(bool enable);

	// the texture at texIdx should already be bound before calling this.
	void setActiveTexture(unsigned int texIdx);
	void setTextureWrap(bool wrap, GLint bindType=GL_TEXTURE_2D); // false means clamp.
	void setTextureMipmap(bool mipmap, GLint bindType = GL_TEXTURE_2D);
	void setTextureHandle(unsigned int texIdx, int handle);

private:
	void onDeviceReinit(void);
	void clearTextureStates(void);

private:
	GHRenderDeviceReinitListener<GHGLESStateCache> mReinitListener;
	friend class GHRenderDeviceReinitListener<GHGLESStateCache>;

	const int* mGraphicsQuality{ 0 };

    // We need to wait for the device to be inited before checking anistropy.
    // Here's a flag to tell if we have already checked.
    bool mCheckedDeviceAnistropy{ false };
	bool mImplementationSupportsAnistropy{ false };
	GLfloat mMaxAnistropy{ 1.0 };

	unsigned int mLastShaderHandle{ 0 }; 
	// 0 might be a valid shader handle.
	bool mShaderValid{ false };

	bool mAlphaEnable{ false };
	bool mAlphaEnableValid{ false };

	GLenum mSrcBlend;
	GLenum mDstBlend;
	bool mAlphaFuncValid{ false };

	bool mZWriteEnable{ true };
	bool mZWriteValid{ false };
	bool mZReadEnable{ true };
	bool mZReadValid{ false };
	bool mZOffset{ false };
	bool mZOffsetValid{ false };

	bool mCullEnable{ true };
	bool mCullValid{ false };
	GLenum mCullFace{ GL_FRONT };
	bool mCullFaceValid{ false };

	// anything > kMaxTrackedTextures we just let through uncached.
	static const int kMaxTrackedTextures = 8;
	unsigned int mActiveTextureIndex{ 0 };
	bool mActiveTextureValid{ false };
	bool mTextureWrap[kMaxTrackedTextures];
	bool mTextureWrapValid[kMaxTrackedTextures];
	int mTextureHandles[kMaxTrackedTextures];
	bool mTextureHandlesValid[kMaxTrackedTextures];
	bool mTextureMip[kMaxTrackedTextures];
	bool mTextureMipValid[kMaxTrackedTextures];
};
