#import "GHMetalVertexStream.h"
#include "GHMetalRenderDevice.h"

GHMetalVertexStream::GHMetalVertexStream(GHVertexStreamFormatPtr* format, unsigned int numVerts, GHMetalRenderDevice& device)
: GHVertexStream(format)
, mDevice(device)
{
    mNumVerts = numVerts;
    size_t byteSize = mNumVerts * format->get()->getVertexSize() * sizeof(float);
    mMetalBuffer = [mDevice.getMetalDevice() newBufferWithLength:byteSize options:MTLResourceStorageModeShared];
}

GHMetalVertexStream::~GHMetalVertexStream(void)
{
    // do we need to delete mMetalBuffer?
}

void GHMetalVertexStream::prepareVB(int streamIdx, GHVertexStreamFormat* formatOverride)
{
    id<MTLRenderCommandEncoder> commandEncoder = mDevice.getRenderCommandEncoder();
    [commandEncoder setVertexBuffer:mMetalBuffer
                             offset:0
                            atIndex:(NSUInteger)streamIdx];
    // formatOverride?
    // todo: glVertexAttribPointer equivalent.
}

void GHMetalVertexStream::endVB(int streamIdx)
{
}

float* GHMetalVertexStream::lockWriteBuffer(void)
{
    return (float*)[mMetalBuffer contents];
}

void GHMetalVertexStream::unlockWriteBuffer(void)
{
    // the changes should get picked up by the gpu automatically.
}

const float* GHMetalVertexStream::lockReadBuffer(void) const
{
    return (float*)[mMetalBuffer contents];
}

void GHMetalVertexStream::unlockReadBuffer(void) const
{
}

GHVertexStream* GHMetalVertexStream::clone(void) const
{
    GHMetalVertexStream* ret = new GHMetalVertexStream(mFormat, mNumVerts, mDevice);
    
    float* retBuffer = ret->lockWriteBuffer();
    const float* srcBuffer = lockReadBuffer();
    memcpy(retBuffer, srcBuffer, mFormat->get()->getVertexSize()*mNumVerts*sizeof(float));
    ret->unlockWriteBuffer();
    return ret;
}
