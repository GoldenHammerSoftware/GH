#import "GHMetalVBBlitterIndex.h"
#include "GHVertexBuffer.h"
#include <vector>
#include "GHMetalShaderStructs.h"
#include "GHMetalToGHConversions.h"

GHMetalVBBlitterIndex::GHMetalVBBlitterIndex(unsigned int numIndices, GHMetalRenderDevice& device)
: GHVBBlitterIndex(numIndices)
, mDevice(device)
{
    size_t byteSize = numIndices * sizeof(unsigned short);
    mMetalBuffer = [mDevice.getMetalDevice() newBufferWithLength:byteSize options:MTLResourceStorageModeShared];
}

GHMetalVBBlitterIndex::~GHMetalVBBlitterIndex(void)
{
}

unsigned short* GHMetalVBBlitterIndex::lockWriteBuffer(void)
{
    return (unsigned short*)[mMetalBuffer contents];
}

void GHMetalVBBlitterIndex::unlockWriteBuffer(void)
{
    // metal should take care of this automatically.
}

const unsigned short* GHMetalVBBlitterIndex::lockReadBuffer(void) const
{
    return (const unsigned short*)[mMetalBuffer contents];
}

void GHMetalVBBlitterIndex::unlockReadBuffer(void) const
{
}

void GHMetalVBBlitterIndex::prepareVB(GHVertexBuffer& vb)
{
    if (!mVertexDescriptor)
    {
        // init the vertex descriptor on the first prepareVB.
        // this is a little shady.  it won't pick up changes.
        
        mVertexDescriptor = [MTLVertexDescriptor new];
        [mVertexDescriptor reset];
        
        const std::vector<GHVertexStreamPtr*>& streams = vb.getStreams();
        for (size_t streamId = 0; streamId < streams.size(); ++streamId)
        {
            GHVertexStreamPtr* stream = streams[streamId];
            const GHVertexStreamFormat& streamFormat = stream->get()->getFormat();
            const std::vector<GHVertexStreamFormat::ComponentEntry>& streamComps = streamFormat.getComponents();
            
            const int bufferIndex = streamId + (int)GHMTL_BI_VERTEXSTREAM0;
            for (size_t compId = 0; compId < streamComps.size(); ++compId)
            {
                const GHVertexStreamFormat::ComponentEntry& compEntry = streamComps[compId];

                int currAttr = GHMetal::getMetalComponentIndex(compEntry.mComponent);
                mVertexDescriptor.attributes[currAttr].bufferIndex = bufferIndex;
                mVertexDescriptor.attributes[currAttr].format = GHMetal::mtlVertexFormatForComp(compEntry);
                mVertexDescriptor.attributes[currAttr].offset = compEntry.mOffset * sizeof(float);
            }
            mVertexDescriptor.layouts[bufferIndex].stride = streamFormat.getVertexSize() * sizeof(float);
            mVertexDescriptor.layouts[bufferIndex].stepFunction = MTLVertexStepFunctionPerVertex;
            mVertexDescriptor.layouts[bufferIndex].stepRate = 1;
        }
    }
    
    const std::vector<GHVertexStreamPtr*>& streams = vb.getStreams();
    for (int i = 0; i < streams.size(); ++i)
    {
        streams[i]->get()->prepareVB(i+GHMTL_BI_VERTEXSTREAM0, 0);
    }
}

void GHMetalVBBlitterIndex::endVB(GHVertexBuffer& vb)
{
    const std::vector<GHVertexStreamPtr*>& streams = vb.getStreams();
    for (int i = 0; i < streams.size(); ++i)
    {
        streams[i]->get()->endVB(i);
    }
}

void GHMetalVBBlitterIndex::blit(void)
{
    id<MTLRenderCommandEncoder> commandEncoder = mDevice.getRenderCommandEncoder();

    [commandEncoder drawIndexedPrimitives:MTLPrimitiveTypeTriangle
                               indexCount:(NSUInteger)mNumIndices
                                indexType:MTLIndexTypeUInt16
                              indexBuffer:mMetalBuffer
                        indexBufferOffset:(NSUInteger)0
                            instanceCount:(NSUInteger)1
                               baseVertex:(NSInteger)0
                             baseInstance:(NSUInteger)0];
}
