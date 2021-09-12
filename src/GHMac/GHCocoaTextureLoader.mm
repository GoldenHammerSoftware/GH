// Copyright 2010 Golden Hammer Software
#include "GHCocoaTextureLoader.h"
#include "GHDebugMessage.h"
#include "GHOGLInclude.h"
#include "GHTextureOGL.h"
#include "GHMutex.h"
#include "GHRenderProperties.h"
#include "GHPropertyContainer.h"

GHCocoaTextureLoader::GHCocoaTextureLoader(GHOGLTextureMgr& textureMgr, GHMutex& renderMutex)
: mTextureMgr(textureMgr)
, mRenderMutex(renderMutex)
, mSupportsDDS(false)
{
	const char* extensions = (const char*)glGetString(GL_EXTENSIONS);
	const char* compARBStr = ::strstr(extensions, "ARB_texture_compression");
	if (!compARBStr) mSupportsDDS = false;
	else
	{
		const char* compEXTStr = ::strstr(extensions, "EXT_texture_compression_s3tc");
		if (compEXTStr) mSupportsDDS = true;
	}
	if (!mSupportsDDS) GHDebugMessage::outputString("Texture compression not supported");
}

GHResource* GHCocoaTextureLoader::loadFile(const char* filename, GHPropertyContainer* extraData)
{
	GHResource* ret = 0;
	
	bool compress = false;
	if (mSupportsDDS)
	{
		const char* isJpg = ::strstr(filename, ".jpg");
		if (isJpg) compress = true;
	}
	
	NSString* path;
	if (!mFileNameCreator.createFileName(filename, path)) 
	{
		GHDebugMessage::outputString("Could not find texture %s", filename);
		return 0;
	}
    
	NSURL* url = [NSURL fileURLWithPath:path];
	
    CFDictionaryRef myOptions = NULL;
    CFStringRef myKeys[2];
    CFTypeRef myValues[2];
	
    myKeys[0] = kCGImageSourceShouldCache;
    myValues[0] = (CFTypeRef)kCFBooleanTrue;
    myKeys[1] = kCGImageSourceShouldAllowFloat;
    myValues[1] = (CFTypeRef)kCFBooleanFalse;
    myOptions = CFDictionaryCreate(NULL, (const void **) myKeys,
								   (const void **) myValues, 2,
								   &kCFTypeDictionaryKeyCallBacks,
								   & kCFTypeDictionaryValueCallBacks);
    
	CGImageSourceRef imageSourceRef = CGImageSourceCreateWithURL((__bridge CFURLRef)url, myOptions);
	CFRelease(myOptions);
    
	if (imageSourceRef == NULL)
	{
		GHDebugMessage::outputString("Image %s source is NULL.", filename);
        return 0;
    }
	
    CGImageRef imageRef = CGImageSourceCreateImageAtIndex(imageSourceRef, 0, NULL);
	CFRelease(imageSourceRef);
    if (imageRef == NULL)
	{
		GHDebugMessage::outputString("Image %s not created from image source.", filename);
		return 0;
    }
	
    // Get the width and height of the image
    unsigned int width = CGImageGetWidth(imageRef);
    unsigned int height = CGImageGetHeight(imageRef);
 	
	// Allocated memory needed for the bitmap context
    unsigned char* spriteData = (unsigned char*) malloc(width * height * 4);
	::memset(spriteData, 0, width*height*4);
    
	mRenderMutex.acquire();
    
    // Uses the bitmatp creation function provided by the Core Graphics framework. 
    CGContextRef spriteContext = CGBitmapContextCreate(spriteData, width, height, 8, width * 4, CGImageGetColorSpace(imageRef), kCGImageAlphaPremultipliedLast);
    // After you create the context, you can draw the sprite image to the context.
    CGContextDrawImage(spriteContext, CGRectMake(0.0, 0.0, (CGFloat)width, (CGFloat)height), imageRef);
    // You don't need the context at this point, so you need to release it to avoid memory leaks.
    CGContextRelease(spriteContext);
    
    bool generateMips = extraData ? !(bool)extraData->getProperty(GHRenderProperties::DONTUSEMIPMAPS) : true;
	ret = new GHTextureOGL(mTextureMgr, spriteData, width, height, 4,
                           generateMips, compress);
    
	mRenderMutex.release();
    
	// NOTE: It is up to the texture whether or not to delete spriteData
	//	if it is deleted then the app can not access the pixels.
	CFRelease(imageRef);
	
	return ret;
}

