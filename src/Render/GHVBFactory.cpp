// Copyright Golden Hammer Software
#include "GHVBFactory.h"
#include "GHVertexBuffer.h"
#include <stddef.h>

GHVertexBuffer* GHVBFactory::createVB(const GHVBDescription& desc) const
{
    GHVertexBuffer* ret = new GHVertexBuffer();
    if (desc.getBlitterType() == GHVBBlitter::BT_INDEX)
    {
        GHVBBlitter* blitter = createIBStream(desc.getNumIndices(), GHVBUsage::STATIC); // todo: nonstatic
        GHVBBlitterPtr* blitterPtr = new GHVBBlitterPtr(blitter);
        ret->replaceBlitter(blitterPtr);
    }
    
    const std::vector<GHVBDescription::StreamDescription>& streamDescs = desc.getStreamDescriptions();
    for (size_t i = 0; i < streamDescs.size(); ++i)
    {
        const GHVBDescription::StreamComponents& streamComps = streamDescs[i].mComps;
        GHVertexStreamFormat* streamFormat = new GHVertexStreamFormat;
        streamFormat->setShared(streamDescs[i].mIsShared);
        
        unsigned int streamVertSize = 0;
        GHVBDescription::StreamComponents::const_iterator compIter;
        for (compIter = streamComps.begin(); compIter != streamComps.end(); ++compIter)
        {
            streamFormat->addComponent(compIter->mID, compIter->mType, compIter->mCount, streamVertSize);
            
            // round to nearest 4byte boundary.
            unsigned int compTypeCount = compIter->mCount;
            unsigned int compSize = compTypeCount;
            if (compIter->mType == GHVertexComponentType::CT_BYTE ||
                compIter->mType == GHVertexComponentType::CT_UBYTE)
            {
                compSize = 1;
                while (compTypeCount > 4)
                {
                    compTypeCount -= 4;
                    compSize++;
                }
            }
            else if (compIter->mType == GHVertexComponentType::CT_SHORT)
            {
                compSize = 1;
                while (compTypeCount > 2)
                {
                    compTypeCount -= 2;
                    compSize++;
                }
            }
            streamVertSize += compSize;
        }
        
        streamFormat->setVertexSize(streamVertSize);
        GHVertexStreamFormatPtr* streamFormatPtr = new GHVertexStreamFormatPtr(streamFormat);
        GHVertexStream* stream = createVBStream(streamFormatPtr, desc.getNumVerts(), streamDescs[i].mUsage);
        ret->replaceStream((unsigned int)i, new GHRefCountedType<GHVertexStream>(stream));
    }
    
    return ret;
}
