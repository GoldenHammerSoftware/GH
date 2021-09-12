// Copyright Golden Hammer Software
#include "GHHeightFieldMeshCreator.h"
#include "GHVBDescription.h"
#include "GHHeightField.h"
#include "GHVBFactory.h"
#include "GHVertexStream.h"
#include "GHVertexBuffer.h"
#include "GHIndexBufferUtil.h"

GHHeightFieldMeshCreator::GHHeightFieldMeshCreator(GHVBFactory& vbFactory)
: mVBFactory(vbFactory)
, mBlitterCache(vbFactory)
{
}

GHVertexBuffer* GHHeightFieldMeshCreator::createVB(const GHHeightField& heightField, const GHPoint2i& meshPos,
                                                   const GHPoint2i& meshSize, const GHPoint2i& meshOrigin,
												   GHRect<float, 3>& bounds, const float* uv) const
{
    unsigned int numQuads = (meshSize[0]-1)*(meshSize[1]-1);
	GHVBDescription vbdesc(meshSize[0] * meshSize[1], 6 * numQuads, GHVBBlitter::BT_UNKNOWN);
    GHVBDescription::StreamDescription streamDesc;
    streamDesc.mIsShared = true;
    streamDesc.mUsage = GHVBUsage::STATIC;
    streamDesc.mComps.push_back(GHVertexComponent::AP_POS);
	if (uv) {
		streamDesc.mComps.push_back(GHVertexComponent::AP_UV1);
	}
    vbdesc.addStreamDescription(streamDesc);
    GHVertexBuffer* ret = mVBFactory.createVB(vbdesc);
	assert(ret);

	GHVBBlitterPtr* sharedBlitter = mBlitterCache.getBlitter(meshSize, vbdesc.getNumIndices());
	ret->replaceBlitter(sharedBlitter);
    
    GHVertexStreamPtr* streamPtr = ret->getStream(0);
    GHVertexStream* stream = streamPtr->get();
    const GHVertexStreamFormat& streamFormat = stream->getFormat();

	unsigned int hfWidth, hfHeight;
	heightField.getDimensions(hfWidth, hfHeight);

    float* streamVerts = stream->lockWriteBuffer();
    const GHVertexStreamFormat::ComponentEntry* posCompEntry = streamFormat.getComponentEntry(GHVertexComponentShaderID::SI_POS);
	const GHVertexStreamFormat::ComponentEntry* uvCompEntry = streamFormat.getComponentEntry(GHVertexComponentShaderID::SI_UV1);
    float nodeDist = heightField.getDistBetweenNodes();
    float yPos = nodeDist*(meshPos[1]-meshOrigin[1]);
    float xStartPos = nodeDist*(meshPos[0]-meshOrigin[0]);
    for (unsigned int row = 0; row < (unsigned int)meshSize[1]; ++row)
    {
        float xPos = xStartPos;
        for (unsigned int col = 0; col < (unsigned int)meshSize[0]; ++col)
        {
            float nodeHeight = heightField.getHeight(meshPos[0]+col, meshPos[1]+row);
            float* vertPos = streamVerts + posCompEntry->mOffset;
            *vertPos = xPos;
            vertPos++;
            *vertPos = nodeHeight;
            vertPos++;
            *vertPos = yPos;
            
            bounds.checkExpandBounds(GHPoint3(xPos, nodeHeight, yPos));
            
			if (uvCompEntry)
			{
				float* uvPos = streamVerts + uvCompEntry->mOffset;
				unsigned int vertIdx = ((meshPos[1] + row)*hfWidth + meshPos[0] + col) * 2;
				if (vertIdx < hfWidth*hfHeight * 2)
				{
					*uvPos = uv[vertIdx];
					uvPos++;
					*uvPos = uv[vertIdx + 1];
				}
			}

            xPos += nodeDist;
            streamVerts += streamFormat.getVertexSize();
        }
        yPos += nodeDist;
    }
    stream->unlockWriteBuffer();
    
    GHVBBlitterPtr* blitterPtr = ret->getBlitter();
    GHVBBlitterIndex* indexBlitter = (GHVBBlitterIndex*)blitterPtr->get();
    GHIndexBufferUtil::fillIndexBufferForGrid(*indexBlitter, meshSize[0], meshSize[1]);
    
    return ret;
}
