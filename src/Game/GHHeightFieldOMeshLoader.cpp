// Copyright Golden Hammer Software
#include "GHHeightFieldOMeshLoader.h"
#include "GHXMLNode.h"
#include "GHTransformNode.h"
#include "GHHeightField.h"
#include "GHGeometryRenderable.h"
#include "GHModel.h"
#include "GHHeightFieldOMeshBucketA.h"
#include "GHHeightFieldMeshShared.h"
#include "GHMath/GHRandom.h"
#include "GHPlatform/GHDebugMessage.h"
#include "GHUtils/GHProfiler.h"
#include "GHHFUVType.h"
#include "GHHeightFieldUVMaker.h"

#include "GHUtils/GHResourceFactory.h"
#include "GHTexture.h"
#include "GHTextureWorldMap.h"
#include "GHUtils/GHPropertyContainer.h"
#include "GHRenderProperties.h"

GHHeightFieldOMeshLoader::GHHeightFieldOMeshLoader(GHXMLObjFactory& objFactory,
                                                   const GHStringIdFactory& hashTable,
                                                   GHVBFactory& vbFactory,
                                                   const GHThreadFactory& threadFactory,
												   const GHIdentifierMap<int>& enumMap,
												   GHResourceFactory& resFactory)
: mObjFactory(objFactory)
, mIdFactory(hashTable)
, mVBFactory(vbFactory)
, mThreadFactory(threadFactory)
, mEnumMap(enumMap)
, mResourceFactory(resFactory)
{
}

void GHHeightFieldOMeshLoader::createRenderable(GHModel& ret, const GHHeightField& hf, 
	const GHXMLNode& node) const
{
	GHPROFILESTART
	GHPROFILEBEGIN("GHHeightFieldOMeshLoader::createRenderable")

	const char* weightTexName = node.getAttribute("weightTexture");
	GHTextureWorldMap* weightMap = 0;
	if (weightTexName) {
		GHPropertyContainer loadProps;
		loadProps.setProperty(GHRenderProperties::GP_KEEPTEXTUREDATA, 1);
		GHTexture* weightTex = (GHTexture*)mResourceFactory.getCacheResource(weightTexName, &loadProps);
		GHPoint2 worldSize;
		unsigned int nodeWidth, nodeHeight;
		hf.getDimensions(nodeWidth, nodeHeight);
		worldSize[0] = hf.getDistBetweenNodes() * (nodeWidth - 1);
		worldSize[1] = hf.getDistBetweenNodes() * (nodeHeight - 1);
		weightMap = new GHTextureWorldMap(weightTex, worldSize, GHPoint2(0, 0));
	}

	GHHFUVType::Enum uvType = GHHFUVType::HFUV_NONE;
	node.readAttrEnum("uvType", (int&)uvType, mEnumMap);
	GHHeightFieldUVMaker uvMaker(hf, uvType);

    std::vector<GHHeightFieldOMeshBucket*> buckets;

    createStarterBuckets(buckets, ret, hf, node, weightMap);
    simplifyMesh(buckets, hf);
    createMeshRenderable(buckets, ret, hf, uvMaker.getUV());
    
//    for (int i = 0; i < buckets.size(); ++i) buckets[i]->debugPrint();
    
    // cleanup.
    for (size_t i = 0; i < buckets.size(); ++i) {
		//GHDebugMessage::outputString("Deleting bucket %d", i);
		// TEST HACK LEAK. tool-only code anyway.
//        delete buckets[i];
    }
	if (weightMap) {
		delete weightMap;
	}
    GHPROFILEEND("GHHeightFieldOMeshLoader::createRenderable")
    GHPROFILESTOP
    GHPROFILEOUTPUT
    GHPROFILECLEAR
}

void GHHeightFieldOMeshLoader::createStarterBuckets(std::vector<GHHeightFieldOMeshBucket*>& buckets,
                                                    GHModel& ret,
                                                    const GHHeightField& hf,
                                                    const GHXMLNode& node,
													GHTextureWorldMap* weightMap) const
{
    GHTransformNode* skeleton = ret.getSkeleton();
    
    char name[128];
    int meshIdx = 0;

    // create min starter mesh buckets for the corner nodes.
    // we need different mesh buckets in order to support multiple materials.
    for (unsigned int i = 0; i < node.getChildren().size(); ++i)
    {
        GHHeightFieldMeshDesc desc;
        if (!desc.loadXML(*node.getChildren()[i], mObjFactory)) {
            continue;
        }
        
		GHPoint2i subPos(desc.getMeshPos());
		GHPoint2i subSize(desc.getMeshSize());
		subSize[0] = (int)((float)subSize[0] / (float)desc.getDivisions()[0]);
		subSize[1] = (int)((float)subSize[1] / (float)desc.getDivisions()[1]);
		for (int x = 0; x < desc.getDivisions()[0]; ++x)
		{
			subPos[1] = desc.getMeshPos()[1];
			for (int y = 0; y < desc.getDivisions()[1]; ++y)
			{
				GHPoint2i segSize(subSize);
				segSize += 1;

				sprintf(name, "hfmesh%d", meshIdx);
				GHIdentifier meshId = mIdFactory.generateHash(name);
				meshIdx++;
				GHTransformNode* meshRoot = GHHeightFieldMeshShared::createMeshNode(desc.getMeshOrigin(), meshId, hf, skeleton);

				GHHeightFieldMeshDesc subDesc(desc);
				subDesc.setMeshPos(subPos);
				subDesc.setMeshSize(segSize);

				GHHeightFieldOMeshBucketA* bucket = new GHHeightFieldOMeshBucketA(subDesc, *meshRoot, hf, mVBFactory, mIdFactory, mThreadFactory, weightMap);
				buckets.push_back(bucket);

				subPos[1] += subSize[1];
			}
			subPos[0] += subSize[0];
		}
    }
}

void GHHeightFieldOMeshLoader::simplifyMesh(std::vector<GHHeightFieldOMeshBucket*>& buckets,
                                            const GHHeightField& hf) const
{
	/*GHDebugMessage::outputString("*** PRE OPTIMIZE ***");
    for (size_t buckIdx = 0; buckIdx < buckets.size(); ++buckIdx)
    {
        buckets[buckIdx]->debugPrint();
    }*/
    
    for (size_t buckIdx = 0; buckIdx < buckets.size(); ++buckIdx)
    {
		GHDebugMessage::outputString("Generating triangles for bucket %d of %d", buckIdx, buckets.size());
		buckets[buckIdx]->generateTriangles(buckets[buckIdx]->getDesc().getVertsPerDivision());
        //buckets[buckIdx]->generateTriangles(500);
    }

	GHDebugMessage::outputString("Fixing bucket edges");
	for (size_t i = 0; i < buckets.size(); ++i) {
		for (size_t j = 0; j < buckets.size(); ++j) {
			if (i == j) continue;
			buckets[i]->shareEdges(*buckets[j]);
		}
	}
	GHPROFILEBEGIN("GHHeightFieldOMeshLoader::validateMesh")
	GHDebugMessage::outputString("Validating meshes.");
	for (size_t i = 0; i < buckets.size(); ++i)
	{
		buckets[i]->validateMesh();
	}
	GHPROFILEEND("GHHeightFieldOMeshLoader::validateMesh")

    GHDebugMessage::outputString("*** POST OPTIMIZE ***");
    for (size_t buckIdx = 0; buckIdx < buckets.size(); ++buckIdx)
    {
        buckets[buckIdx]->debugPrint();
    }
}

void GHHeightFieldOMeshLoader::createMeshRenderable(std::vector<GHHeightFieldOMeshBucket*>& buckets,
                                                    GHModel& ret,
                                                    const GHHeightField& hf,
													const float* uv) const
{
    // and generate the renderable.
    GHGeometryRenderable* renderable = new GHGeometryRenderable();
    ret.setRenderable(renderable);
    int meshIdx = 0;
    for (size_t i = 0; i < buckets.size(); ++i)
    {
        buckets[i]->createGeo(*renderable, meshIdx, uv);
    }
}
