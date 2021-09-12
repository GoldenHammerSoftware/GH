#ifndef GHMETAL

// Copyright 2010 Golden Hammer Software
// code modified from http://developer.apple.com/iphone/library/samplecode/PVRTextureLoader/listing8.html
#include "GHiOSTextureLoaderPVRTC.h"
#include "GHGLESInclude.h"
#include "GHPlatform/GHDebugMessage.h"
#include "GHGLESTextureMgr.h"
#include "GHGLESErrorReporter.h"
#include "GHPlatform/GHMutex.h"
#include "GHPVRUtil.h"
#include "GHUtils/GHProfiler.h"
#include "GHTextureData.h"
#include "GHGLESTexture.h"

GHiOSTextureLoaderPVRTC::GHiOSTextureLoaderPVRTC(GHGLESTextureMgr& textureMgr, 
                                                 GHMutex* mutex,
                                                 const GHFileOpener& fileOpener,
                                                 GHGLESStateCache& stateCache)
: mTextureMgr(textureMgr)
, mMutex(mutex)
, mFileOpener(fileOpener)
, mStateCache(stateCache)
{
}

static void createGLTexture(unsigned int texId, GHPVRUtil::PVRTextureDesc& tex)
{
	glBindTexture(GL_TEXTURE_2D, texId);
	GHGLESErrorReporter::checkAndReportError("glBindTexture");
	
	int width = tex._width;
	int height = tex._height;
	void* mipData;
	for (int i=0; i < tex._imageData.size(); i++)
	{
		mipData = tex._imageData[i];
		GLsizei mipLength = (GLsizei)tex._imageDataSize[i];
		glCompressedTexImage2D(GL_TEXTURE_2D, i, tex._internalFormat, width, height, 0, mipLength, mipData);
		GHGLESErrorReporter::checkAndReportError("glCompressedTexImage2D");
		
		width = MAX(width >> 1, 1);
		height = MAX(height >> 1, 1);
		//[mipData release];
	}
}

GHResource* GHiOSTextureLoaderPVRTC::loadFile(const char* filename, GHPropertyContainer* extraData)
{
    GHPROFILESCOPE("PVRLoad", GHString::CHT_REFERENCE)

    @autoreleasepool {
        void* data = 0;
        size_t dataSize = 0;
        bool openedFile = GHPVRUtil::loadPVRData(filename, mFileNameCreator, mFileOpener, data, dataSize);
        if (!openedFile) {
            return 0;
        }
        
        GHPVRUtil::PVRTextureDesc tex;
        tex._name = 0;
        tex._width = tex._height = 0;
        tex._internalFormat = GHTextureFormat::TF_PVRTC_4BPP_RGBA;
        tex._hasAlpha = FALSE;
        
        bool unpacked = GHPVRUtil::unpackData(data, dataSize, tex,
                                              GHTextureFormat::TF_PVRTC_4BPP_RGBA,
                                              GHTextureFormat::TF_PVRTC_4BPP_RGB,
                                              GHTextureFormat::TF_PVRTC_2BPP_RGBA,
                                              GHTextureFormat::TF_PVRTC_2BPP_RGB);
        if (!unpacked) {
            GHDebugMessage::outputString("Failed to unpack pvr texture.\n");
            return 0;
        }
        
        if (mMutex) mMutex->acquire();
        
        GHTextureData* texData = new GHTextureData();
        texData->mChannelType = GHTextureChannelType::TC_UBYTE;
        texData->mTextureFormat = (GHTextureFormat::Enum)tex._internalFormat;
        texData->mSrgb = false;
        texData->mDepth = 4;
        
        texData->mMipLevels.resize(tex._imageData.size());
        
        int width = tex._width;
        int height = tex._height;
        void* mipData;
        for (int i=0; i < tex._imageData.size(); i++)
        {
            mipData = tex._imageData[i];
            texData->mMipLevels[i].mData = mipData;
            texData->mMipLevels[i].mDataSize = tex._imageDataSize[i];
            texData->mMipLevels[i].mWidth = width;
            texData->mMipLevels[i].mHeight = height;
            
            width = MAX(width >> 1, 1);
            height = MAX(height >> 1, 1);
        }
        
        texData->mDataSource = (int8_t*)data;
        
        GHGLESTexture* ret = new GHGLESTexture(mTextureMgr, mStateCache, texData, false, 0, 0);
        
        // deleteSourceData will delete texData which will delete the file buffer.
        ret->deleteSourceData();
        
        if (mMutex) mMutex->release();
        return ret;
        
    } // autoreleasepool
}

#endif
