#include "GHMetalCubemapXMLLoader.h"
#include "GHResourceFactory.h"
#include "GHMetalTexture.h"
#include "GHGhcmXmlParser.h"
#include "GHPlatform/GHDebugMessage.h"
#include "GHMetalRenderDevice.h"

GHMetalCubemapXMLLoader::GHMetalCubemapXMLLoader(GHResourceFactory& resourceFactory, GHMetalRenderDevice& device)
: mResourceFactory(resourceFactory)
, mDevice(device)
{
    
}

void* GHMetalCubemapXMLLoader::create(const GHXMLNode& node, GHPropertyContainer& extraData) const
{
    GHTexture* faceArray[6];
    bool parsed = GHGhcmXmlParser::parseGHCM(mResourceFactory, node, extraData, faceArray);
    if (!parsed)
    {
        GHDebugMessage::outputString("Failed to parse cubemap faces");
    }
    GHMetalTexture* metalTex = (GHMetalTexture*)faceArray[0];
    unsigned int width, height, depth;
    bool gotDims = metalTex->getDimensions(width, height, depth);
    if (!gotDims)
    {
        return 0;
    }
    assert(width == height);
    assert(depth != 0); // todo: compressed formats using getBlockWidthPixels.
    
    // Make a dest texture.
    MTLTextureDescriptor* desc = [MTLTextureDescriptor textureCubeDescriptorWithPixelFormat:metalTex->getMetalTexture().pixelFormat size:width mipmapped:YES];
    //desc.usage = MTLTextureUsageShaderRead | MTLTextureUsageShaderWrite;
    id<MTLTexture> texture = [mDevice.getMetalDevice() newTextureWithDescriptor:desc];
    
    NSMutableString* cubeName = [[NSMutableString alloc] init];
    [cubeName appendString:metalTex->getMetalTexture().label];
    [cubeName appendString:@"cubemap"];
    texture.label = cubeName;
    
//#define GPU_COPY_CUBEMAP 1
#ifndef GPU_COPY_CUBEMAP
    char* readBuf = new char[width*height*depth];
    for (int face = 0; face < 6; ++face)
    {
        GHMetalTexture* sourceGHTex = (GHMetalTexture*)faceArray[face];
        id<MTLTexture> sourceMetalTex = sourceGHTex->getMetalTexture();
        int mipWidth = width;
        int mipHeight = height;
        for (int mip = 0; mip < sourceMetalTex.mipmapLevelCount; ++mip)
        {
            MTLRegion region = MTLRegionMake2D(0,0,mipWidth,mipHeight);
            int bytesPerRow = mipWidth*depth;
            [sourceMetalTex getBytes:readBuf bytesPerRow:bytesPerRow fromRegion:region mipmapLevel:mip];
            [texture replaceRegion:region mipmapLevel:mip slice:face withBytes:readBuf bytesPerRow:bytesPerRow bytesPerImage:0];
            mipWidth /= 2;
            mipHeight /= 2;
        }
    }
    delete [] readBuf;
#else
    // copy faces
    id<MTLCommandQueue> commandQueue = mDevice.getCommandQueue();
    id<MTLCommandBuffer> commandBuffer = [commandQueue commandBuffer];
    id<MTLBlitCommandEncoder> blitEncoder = [commandBuffer blitCommandEncoder];
    for (int i = 0; i < 6; ++i)
    {
        GHMetalTexture* sourceGHTex = (GHMetalTexture*)faceArray[i];
        id<MTLTexture> sourceMetalTex = sourceGHTex->getMetalTexture();
        for (int mip = 0; mip < sourceMetalTex.mipmapLevelCount; ++mip)
        {
            [blitEncoder copyFromTexture:sourceMetalTex sourceSlice:0 sourceLevel:mip toTexture:texture destinationSlice:i destinationLevel:mip sliceCount:1 levelCount:1];
        }
    }
    [blitEncoder endEncoding];
    [commandBuffer commit];
    [commandBuffer waitUntilCompleted];
#endif
    
    for (int i = 0; i < 6; ++i)
    {
        GHMetalTexture* sourceGHTex = (GHMetalTexture*)faceArray[i];
        sourceGHTex->release();
    }
    
    GHMetalTexture* ret = new GHMetalTexture(texture, mDevice, true);
    return ret;
}
