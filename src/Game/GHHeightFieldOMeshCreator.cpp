// Copyright Golden Hammer Software
#include "GHHeightFieldOMeshCreator.h"
#include "GHVBDescription.h"
#include "GHVBFactory.h"
#include "GHVertexStream.h"
#include "GHVertexBuffer.h"
#include "GHHeightField.h"
#include <float.h>
#include "GHMath/GHRect.h"
#include "GHHeightFieldMeshDesc.h"
#include "GHGeometry.h"
#include "GHGeometryRenderable.h"
#include "GHSphereBounds.h"
#include "GHUtils/GHProfiler.h"
#include "GHPlatform/GHDebugMessage.h"

GHHeightFieldOMeshCreator::GHHeightFieldOMeshCreator(const GHHeightFieldMeshDesc& desc, GHTransformNode& topNode,
                                                     const GHHeightField& hf, GHVBFactory& vbFactory,
                                                     const GHStringIdFactory& hashTable)
: mDesc(desc)
, mTopNode(topNode)
, mHF(hf)
, mVBFactory(vbFactory)
, mIdFactory(hashTable)
{
}

void GHHeightFieldOMeshCreator::createGeo(GHGeometryRenderable& renderable, int& meshIdx,
                                          const std::vector<MeshTri>& tris, 
										  const float* uv) const
{
    GHPROFILESCOPE("GHHeightFieldOMeshCreator::createGeo", GHString::CHT_REFERENCE)
    
    std::vector<unsigned int> indicies;
    std::vector<VertPos> vertIds;
    vertIds.reserve(USHRT_MAX);
    
    size_t triId = 0;
    while (triId < tris.size())
    {
        indicies.resize(0);
        vertIds.resize(0);
        
        // need to make sure we don't make any single geometry with more verts than can fit in an unsigned short.
        while (vertIds.size() < USHRT_MAX - 3 && triId < tris.size())
        {
            for (size_t vertId = 0; vertId < 3; ++vertId)
            {
                int storedVert = -1;
                // filter out verts that are already added.
                for (size_t testVert = 0; testVert < vertIds.size(); ++testVert)
                {
                    if (vertIds[testVert] == tris[triId].mVerts[vertId]) {
                        storedVert = (int)testVert;
                        break;
                    }
                }
                
                if (storedVert == -1) {
                    // new vert.
                    vertIds.push_back(tris[triId].mVerts[vertId]);
                    storedVert = (int)(vertIds.size())-1;
                }
                
                indicies.push_back(storedVert);
            }
            triId++;
        }
        assert(vertIds.size() < USHRT_MAX);
        
		createGeoForArrays(vertIds, indicies, renderable, meshIdx, uv);
        meshIdx++;
    }
}

void GHHeightFieldOMeshCreator::createGeoForArrays(std::vector<VertPos>& vertIds, std::vector<unsigned int>& indicies,
                                                   GHGeometryRenderable& renderable, int meshIdx,
												   const float* uv) const
{
    GHVBDescription vbdesc((unsigned int)vertIds.size(), (unsigned int)indicies.size(), GHVBBlitter::BT_INDEX);
    GHVBDescription::StreamDescription streamDesc;
    streamDesc.mIsShared = true;
    streamDesc.mUsage = GHVBUsage::STATIC;
    streamDesc.mComps.push_back(GHVertexComponent::AP_POS);
	if (uv) {
		streamDesc.mComps.push_back(GHVertexComponent::AP_UV1);
	}
    vbdesc.addStreamDescription(streamDesc);
    GHVertexBuffer* vb = mVBFactory.createVB(vbdesc);
    
    GHVertexStreamPtr* streamPtr = vb->getStream(0);
    GHVertexStream* stream = streamPtr->get();
    const GHVertexStreamFormat& streamFormat = stream->getFormat();
    
    float* streamVerts = stream->lockWriteBuffer();
    const GHVertexStreamFormat::ComponentEntry* posCompEntry = streamFormat.getComponentEntry(GHVertexComponentShaderID::SI_POS);
	const GHVertexStreamFormat::ComponentEntry* uvCompEntry = streamFormat.getComponentEntry(GHVertexComponentShaderID::SI_UV1);
    float nodeDist = mHF.getDistBetweenNodes();
    
    GHPoint3 startMin(FLT_MAX, FLT_MAX, FLT_MAX);
    GHPoint3 startMax(FLT_MIN, FLT_MIN, FLT_MIN);
    GHRect<float, 3> bounds(startMin, startMax);
    
	unsigned int hfWidth, hfHeight;
	mHF.getDimensions(hfWidth, hfHeight);

    for (size_t vertId = 0; vertId < vertIds.size(); ++vertId)
    {
        float* posStart = streamVerts + posCompEntry->mOffset;
        
        float x = (vertIds[vertId][0]-mDesc.getMeshOrigin()[0])*nodeDist;
        float y = (vertIds[vertId][1]-mDesc.getMeshOrigin()[1])*nodeDist;
        float z = mHF.getHeight(vertIds[vertId][0], vertIds[vertId][1]);
        
        *posStart = x;
        posStart++;
        *posStart = z;
        posStart++;
        *posStart = y;
        
        bounds.checkExpandBounds(GHPoint3(x, z, y));
        
		if (uvCompEntry)
		{
			float* uvPos = streamVerts + uvCompEntry->mOffset;
			unsigned int vertIdx = vertIds[vertId][0] * 2 + vertIds[vertId][1] * hfWidth * 2;
			if (vertIdx < hfWidth*hfHeight * 2)
			{
				*uvPos = uv[vertIdx];
				uvPos++;
				*uvPos = uv[vertIdx + 1];
			}
		}

        streamVerts += streamFormat.getVertexSize();
    }
    
    stream->unlockWriteBuffer();
    
    GHVBBlitterIndex* indexBlitter = (GHVBBlitterIndex*)(vb->getBlitter()->get());
    unsigned short* ib = indexBlitter->lockWriteBuffer();
    for (size_t i = 0; i < indicies.size(); ++i)
    {
        *ib = indicies[i];
        ib++;
    }
    indexBlitter->unlockWriteBuffer();
    
    GHGeometry* geo = new GHGeometry;
    renderable.addGeometry(geo);
    geo->setVB(new GHVertexBufferPtr(vb));
    geo->setMaterial(mDesc.getMaterial());
    
    char name[256];
    sprintf(name, "hfmesh%d", meshIdx);
    GHIdentifier meshId = mIdFactory.generateHash(name);
    geo->setId(meshId);
    geo->setTransform(&mTopNode);
    
    GHSphereBounds* sphereBounds = new GHSphereBounds;
    GHSphere sphere;
    sphere.fromAABB(bounds);
    sphereBounds->setSphere(sphere);
    geo->setBounds(sphereBounds);
    
	//GHDebugMessage::outputString("Box (%f, %f, %f) (%f, %f, %f)", bounds.mMin[0], bounds.mMin[1], bounds.mMin[2], bounds.mMax[0], bounds.mMax[1], bounds.mMax[2]);
	//GHDebugMessage::outputString("Sphere (%f,%f,%f) rad %f", sphere.mCenter[0], sphere.mCenter[1], sphere.mCenter[2], sphere.mRadius);
    //GHDebugMessage::outputString("Created hf geo with %d tris and %d verts", indicies.size()/3, vertIds.size());
}
