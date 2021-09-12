#include "GHMetalTextureLoader.h"
#include "GHMetalRenderDevice.h"
#include "GHPlatform/GHDebugMessage.h"
#include "GHMetalTexture.h"

GHMetalTextureLoader::GHMetalTextureLoader(GHMetalRenderDevice& device)
: mDevice(device)
{
    mMTKLoader = [[MTKTextureLoader alloc] initWithDevice:mDevice.getMetalDevice()];
}

GHMetalTextureLoader::~GHMetalTextureLoader(void)
{
    for (auto iter = mOverrideLoaders.begin(); iter != mOverrideLoaders.end(); ++iter)
    {
        delete *iter;
    }
    mOverrideLoaders.clear();
}

GHResource* GHMetalTextureLoader::loadFile(const char* filename, GHPropertyContainer* extraData)
{
    // first look for file format overrides
    for (auto iter = mOverrideLoaders.begin(); iter != mOverrideLoaders.end(); ++iter)
    {
        GHResourceLoader* loader = *iter;
        GHResource* overRet = loader->loadFile(filename);
        if (overRet) {
            return overRet;
        }
    }
    
    // didn't find an override version, so load natural.
    NSString* path;
    if (!mFileNameCreator.createFileName(filename, path)) {
        return 0;
    }
    NSURL* url = [NSURL fileURLWithPath: path];
    NSDictionary<MTKTextureLoaderOption, id> *options = nil;
    if (strstr(filename, "png"))
    {
        options = @{ MTKTextureLoaderOptionGenerateMipmaps : @YES, MTKTextureLoaderOptionSRGB : @NO };
    }
    NSError* error = nil;
    id<MTLTexture> texture = [mMTKLoader newTextureWithContentsOfURL:url options:options error:&error];
    if(!texture)
    {
        GHDebugMessage::outputString("Failed to create mtk texture");
        return 0;
    }
    if (error)
    {
        GHDebugMessage::outputString("Error loading mtk texture");
    }

    return new GHMetalTexture(texture, mDevice, true);
}

void GHMetalTextureLoader::addOverrideLoader(GHResourceLoader* loader)
{
    mOverrideLoaders.push_back(loader);
}
