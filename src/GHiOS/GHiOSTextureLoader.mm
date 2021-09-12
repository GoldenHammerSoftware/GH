// Copyright 2010 Golden Hammer Software
#include "GHiOSTextureLoader.h"
#include "GHPlatform/GHDebugMessage.h"
#include "GHPlatform/GHMutex.h"
#include "GHGLESTexture.h"
#include "GHUtils/GHProfiler.h"
#include "GHTextureData.h"
#include "GHUtils/GHResourceLoader.h"
#include "GHUtils/GHRedirectResourceLoader.h"

#import <QuartzCore/QuartzCore.h>
#import <OpenGLES/EAGLDrawable.h>
#import <UIKit/UIKit.h>

GHiOSTextureLoader::GHiOSTextureLoader(GHGLESTextureMgr& textureMgr, GHMutex* mutex,
                                       GHResourceFactory& cache, GHGLESStateCache& stateCache)
: mTextureMgr(textureMgr)
, mMutex(mutex)
, mStateCache(stateCache)
{
    addOverrideLoader(new GHRedirectResourceLoader(cache, ".pvr"));
}

GHiOSTextureLoader::~GHiOSTextureLoader(void)
{
    for (auto iter = mOverrideLoaders.begin(); iter != mOverrideLoaders.end(); ++iter)
    {
        delete *iter;
    }
    mOverrideLoaders.clear();
}
void GHiOSTextureLoader::addOverrideLoader(GHResourceLoader* loader)
{
    mOverrideLoaders.push_back(loader);
}

GHResource* GHiOSTextureLoader::loadFile(const char* filename, GHPropertyContainer* extraData)
{
    //char debugStr[512];
    //::sprintf(debugStr, "TextureLoad: %s", filename);
    //GHPROFILESCOPE(debugStr, GHString::CHT_COPY);
    GHPROFILESCOPE("TextureLoad", GHString::CHT_REFERENCE)
    
    // first check to see if there is a pvr version.
    for (auto iter = mOverrideLoaders.begin(); iter != mOverrideLoaders.end(); ++iter)
    {
        GHResourceLoader* loader = *iter;
        GHResource* pvrRet = loader->loadFile(filename);
        if (pvrRet) {
            return pvrRet;
        }
    }
    
	NSString* path;
	if (!mFileNameCreator.createFileName(filename, path)) {
        path = [[NSString alloc] initWithUTF8String:filename];
		//return 0;
	}
 	
    // Creates a Core Graphics image from an image file
	UIImage* spriteUIImage = [[UIImage alloc] initWithContentsOfFile:path];
	if (!spriteUIImage) {
		GHDebugMessage::outputString("Failed to load source texture %s.", filename);
		return 0;
	}
    CGImageRef spriteImage = spriteUIImage.CGImage;
    // Get the width and height of the image
    unsigned int width = spriteUIImage.size.width;
    unsigned int height = spriteUIImage.size.height;
 	
    if (spriteImage) 
	{
        // Allocated memory needed for the bitmap context
        unsigned char* spriteData = (unsigned char*) malloc(width * height * 4);
		::memset(spriteData, 0, width*height*4);
		
		if (mMutex) mMutex->acquire();
		
        // Uses the bitmatp creation function provided by the Core Graphics framework. 
        CGContextRef spriteContext = CGBitmapContextCreate(spriteData, width, height, 8, width * 4, CGImageGetColorSpace(spriteImage), kCGImageAlphaPremultipliedLast);
        // After you create the context, you can draw the sprite image to the context.
        CGContextDrawImage(spriteContext, CGRectMake(0.0, 0.0, (CGFloat)width, (CGFloat)height), spriteImage);
        // You don't need the context at this point, so you need to release it to avoid memory leaks.
        CGContextRelease(spriteContext);
		
        // todo: support not generating mipmaps
        bool generateMips = true;
        
        GHTextureData* texData = new GHTextureData();
        texData->mChannelType = GHTextureChannelType::TC_UBYTE;
        texData->mTextureFormat = GHTextureFormat::TF_RGBA8;
        texData->mSrgb = false;
        texData->mDepth = 4;
        
        texData->mMipLevels.resize(1);
        texData->mMipLevels[0].mWidth = width;
        texData->mMipLevels[0].mHeight = height;
        texData->mMipLevels[0].mDataSize = width * height * 4;
        texData->mMipLevels[0].mData = spriteData;
        
        texData->mDataSource = (int8_t*)spriteData;
        
        GHGLESTexture* ret = new GHGLESTexture(mTextureMgr, mStateCache, texData, generateMips, 0, 0);
		
		if (mMutex) mMutex->release();
		
		// NOTE: It is up to the texture whether or not to delete spriteData
		//	if it is deleted then the app can not access the pixels.
		return ret;
    }
	
	GHDebugMessage::outputString("spriteUIImage %s has no CGImage.");
	return 0;
}
