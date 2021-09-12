#include "GHMetalTexture.h"
#include "GHTextureDataLoader.h"
#include "GHTextureData.h"
#include "GHMetalRenderDevice.h"
#include "GHMetalToGHConversions.h"
#include "GHTextureTypeUtil.h"

GHMetalTexture::GHMetalTexture(id<MTLTexture> metalTexture,
                               GHMetalRenderDevice& device,
                               bool mipmap)
: mMetalTexture(metalTexture)
, mDevice(device)
, mMipmap(mipmap)
{
}

GHMetalTexture::~GHMetalTexture(void)
{
}

void GHMetalTexture::setBindId(int id)
{
    mBindId = id;
}

void GHMetalTexture::setWrapMode(GHMDesc::WrapMode wrapMode)
{
    if (mMetalSampler && mWrapMode == wrapMode)
    {
        // no changes, no work to do.
        // if we have a texture that is flip flopping wrap modes,
        //  this could be slow.  otherwise not worth worrying about.
        return;
    }
    mWrapMode = wrapMode;
    
    MTLSamplerDescriptor* desc = [MTLSamplerDescriptor new];
    if (mMipmap)
    {
        desc.minFilter = MTLSamplerMinMagFilterLinear;
        desc.magFilter = MTLSamplerMinMagFilterLinear;
        desc.mipFilter = MTLSamplerMipFilterLinear;
        desc.maxAnisotropy = 4;
    }
    else
    {
        desc.minFilter = MTLSamplerMinMagFilterNearest;
        desc.magFilter = MTLSamplerMinMagFilterNearest;
        desc.mipFilter = MTLSamplerMipFilterNotMipmapped;
    }
    
    if (wrapMode == GHMDesc::WM_WRAP)
    {
        desc.rAddressMode = MTLSamplerAddressModeRepeat;
        desc.sAddressMode = MTLSamplerAddressModeRepeat;
        desc.tAddressMode = MTLSamplerAddressModeRepeat;
    }
    else
    {
        desc.rAddressMode = MTLSamplerAddressModeClampToEdge;
        desc.sAddressMode = MTLSamplerAddressModeClampToEdge;
        desc.tAddressMode = MTLSamplerAddressModeClampToEdge;
    }
    mMetalSampler = [mDevice.getMetalDevice() newSamplerStateWithDescriptor:desc];
}

void GHMetalTexture::bind(void)
{
    [mDevice.getRenderCommandEncoder() setFragmentTexture:mMetalTexture atIndex:mBindId];
    [mDevice.getRenderCommandEncoder() setFragmentSamplerState:mMetalSampler atIndex:mBindId];
}

bool GHMetalTexture::lockSurface(void** ret, unsigned int& pitch)
{
    if (!mMetalTexture)
    {
        return false;
    }
    GHTextureFormat::Enum ghFormat = GHMetal::GHPixelFormatForMetal(mMetalTexture.pixelFormat);
    if (GHTextureTypeUtil::isCompressedType(ghFormat))
    {
        assert(0);
        return false;
    }
    unsigned int width, height, depth;
    if (!getDimensions(width, height, depth))
    {
        return false;
    }
    pitch = width * depth;
    
    if (!mSourceData)
    {
        mSourceData = new char(width*height*depth);
        // this seems to crash
        //[mMetalTexture getBytes:mSourceData bytesPerRow:pitch fromRegion:MTLRegionMake2D(0,0,width, height) mipmapLevel:0];
    }
    *ret = mSourceData;
    return true;
}

bool GHMetalTexture::unlockSurface(void)
{
    // just hang on to our source data until deleteSourceData is called.
    // this allows lockSurface to be fast.
    // could go wrong if people are expecting to be able to write.
    return true;
}

bool GHMetalTexture::getDimensions(unsigned int& width, unsigned int& height, unsigned int& depth)
{
    if (!mMetalTexture)
    {
        return false;
    }
    width = (unsigned int)mMetalTexture.width;
    height = (unsigned int)mMetalTexture.height;

    GHTextureFormat::Enum ghFormat = GHMetal::GHPixelFormatForMetal(mMetalTexture.pixelFormat);
    depth = (unsigned int)GHTextureTypeUtil::getPixelSizeBytes(ghFormat);

    return true;
}

void GHMetalTexture::deleteSourceData(void)
{
    if (mSourceData)
    {
        delete [] (char*)mSourceData;
        mSourceData = 0;
    }
}

