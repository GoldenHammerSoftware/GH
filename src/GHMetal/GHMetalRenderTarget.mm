#include "GHMetalRenderTarget.h"
#include "GHMetalRenderDevice.h"
#include "GHMetalTexture.h"

GHMetalRenderTarget::GHMetalRenderTarget(const GHRenderTarget::Config& args, GHMetalRenderDevice& device)
: mConfig(args)
, mDevice(device)
{
    initTextures();
}

GHMetalRenderTarget::~GHMetalRenderTarget(void)
{
}

void GHMetalRenderTarget::apply(void)
{
}

void GHMetalRenderTarget::remove(void)
{
    if (mConfig.mMipmap)
    {
        // we only rendered into mip 0, so generate the other mips with a blit encoder.
        id<MTLCommandQueue> commandQueue = mDevice.getCommandQueue();
        id<MTLCommandBuffer> commandBuffer = [commandQueue commandBuffer];
        id<MTLBlitCommandEncoder> blitEncoder = [commandBuffer blitCommandEncoder];

        [blitEncoder generateMipmapsForTexture:mTexture->getMetalTexture()];
        
        [blitEncoder endEncoding];
        [commandBuffer commit];
        // possibly not needed.
        //[commandBuffer waitUntilCompleted];
    }
}

GHTexture* GHMetalRenderTarget::getTexture(void)
{
    if (mConfig.mType == GHRenderTarget::RT_DEPTHONLY)
    {
        // this doesn't really work on ios.
        //return mDepthTexture;
    }
    return mTexture;
}

void GHMetalRenderTarget::resize(const GHRenderTarget::Config& args)
{
    mConfig = args;
    initTextures();
}

void GHMetalRenderTarget::initTextures(void)
{
    if (mTexture) mTexture->release();
    if (mMsaaTexture) mMsaaTexture->release();
    
    if (mDevice.getMsaaSampleCount() > 1)
    {
        mTexture = initColorTarget(1);
        mMsaaTexture = initColorTarget(mDevice.getMsaaSampleCount());
    }
    else
    {
        mTexture = initColorTarget(1);
    }
    initDepthTarget();
    initRenderPassDescriptor();
}

GHMetalTexture* GHMetalRenderTarget::initColorTarget(int sampleCount)
{
    MTLTextureDescriptor* texDesc = [[MTLTextureDescriptor alloc] init];
    texDesc.width = mConfig.mWidth;
    texDesc.height = mConfig.mHeight;
    texDesc.usage = MTLTextureUsageShaderRead | MTLTextureUsageRenderTarget;
    texDesc.sampleCount = sampleCount;
    
    if (sampleCount > 1)
    {
        texDesc.storageMode = MTLStorageModePrivate;
        texDesc.textureType = MTLTextureType2DMultisample;
    }
    // msaa textures don't have the mipmap, that goes in the resolve tex.
    else if (mConfig.mMipmap)
    {
        int heightLevels = ceil(log2(texDesc.height));
        int widthLevels = ceil(log2(texDesc.width));
        int mipCount = (heightLevels > widthLevels) ? heightLevels : widthLevels;
        texDesc.mipmapLevelCount = mipCount;
    }

    // todo: different format for depthonly
 //   if (mConfig.mType == GHRenderTarget::RT_DEPTHONLY)
//    {
//        texDesc.pixelFormat = MTLPixelFormatR32Float;
//    }
//    else
    {
        texDesc.pixelFormat = MTLPixelFormatBGRA8Unorm;
    }
    
    id<MTLTexture> mtlTex = [mDevice.getMetalDevice() newTextureWithDescriptor:texDesc];
    mtlTex.label = [[NSString alloc] initWithCString:mConfig.mLabel.getChars() encoding:NSASCIIStringEncoding];
    return new GHMetalTexture(mtlTex, mDevice, mConfig.mMipmap);
}

void GHMetalRenderTarget::initDepthTarget(void)
{
    MTLTextureDescriptor* depthDesc = [[MTLTextureDescriptor alloc] init];
    depthDesc.width = mConfig.mWidth;
    depthDesc.height = mConfig.mHeight;
    depthDesc.pixelFormat = MTLPixelFormatDepth32Float;
    depthDesc.usage = MTLTextureUsageShaderRead | MTLTextureUsageRenderTarget;
    depthDesc.storageMode = MTLStorageModePrivate;
    
    if (mDevice.getMsaaSampleCount() > 1)
    {
        depthDesc.sampleCount = mDevice.getMsaaSampleCount();
        depthDesc.textureType = MTLTextureType2DMultisample;
    }
    else
    {
        depthDesc.sampleCount = 1;
    }
    
    id<MTLTexture> depthTex = [mDevice.getMetalDevice() newTextureWithDescriptor:depthDesc];
    if (mDepthTexture)
    {
        mDepthTexture->release();
    }
    depthTex.label = [[NSString alloc] initWithCString:mConfig.mLabel.getChars() encoding:NSASCIIStringEncoding];
    mDepthTexture = new GHMetalTexture(depthTex, mDevice, false);
}

void GHMetalRenderTarget::initRenderPassDescriptor(void)
{
    if (!mRenderPassDescriptor)
    {
        mRenderPassDescriptor = [MTLRenderPassDescriptor renderPassDescriptor];
    }

    if (mDevice.getMsaaSampleCount() > 1)
    {
        mRenderPassDescriptor.colorAttachments[0].texture = mMsaaTexture->getMetalTexture();
        mRenderPassDescriptor.colorAttachments[0].resolveTexture = mTexture->getMetalTexture();

        mRenderPassDescriptor.colorAttachments[0].loadAction = MTLLoadActionClear;
        mRenderPassDescriptor.colorAttachments[0].storeAction = MTLStoreActionStoreAndMultisampleResolve;
        mRenderPassDescriptor.colorAttachments[0].clearColor = MTLClearColorMake(0, 0, 0, 0);
    }
    else
    {
        mRenderPassDescriptor.colorAttachments[0].texture = mTexture->getMetalTexture();
        mRenderPassDescriptor.colorAttachments[0].loadAction = MTLLoadActionClear;
        mRenderPassDescriptor.colorAttachments[0].storeAction = MTLStoreActionStore;
        mRenderPassDescriptor.colorAttachments[0].clearColor = MTLClearColorMake(0, 0, 0, 0);
    }
    
    mRenderPassDescriptor.depthAttachment.texture = mDepthTexture->getMetalTexture();
    mRenderPassDescriptor.depthAttachment.loadAction = MTLLoadActionClear;
    mRenderPassDescriptor.depthAttachment.storeAction = MTLStoreActionStore;
    mRenderPassDescriptor.depthAttachment.clearDepth = 1.0;
}
