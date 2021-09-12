// Copyright Golden Hammer Software
#include "GHHeightFieldNMeshLoader.h"
#include "GHGeometryRenderable.h"
#include "GHModel.h"
#include "GHXMLNode.h"
#include "GHPlatform/GHDebugMessage.h"
#include "GHXMLObjFactory.h"
#include "GHHeightField.h"
#include "GHGeometry.h"
#include <float.h>
#include "GHHeightFieldMeshDesc.h"
#include "GHHeightFieldMeshShared.h"
#include "GHHeightFieldUVMaker.h"

GHHeightFieldNMeshLoader::GHHeightFieldNMeshLoader(GHXMLObjFactory& objFactory, const GHStringIdFactory& hashTable,
	GHVBFactory& vbFactory, const GHIdentifierMap<int>& enumMap)
: mObjFactory(objFactory)
, mIdFactory(hashTable)
, mMeshCreator(vbFactory)
, mEnumMap(enumMap)
{
}

void GHHeightFieldNMeshLoader::createRenderable(GHModel& ret, const GHHeightField& hf, const GHXMLNode& node) const
{
	GHHFUVType::Enum uvType = GHHFUVType::HFUV_NONE;
	node.readAttrEnum("uvType", (int&)uvType, mEnumMap);
	GHHeightFieldUVMaker uvMaker(hf, uvType);

    GHGeometryRenderable* renderable = new GHGeometryRenderable();
    ret.setRenderable(renderable);
    int meshId = 0;
    for (size_t i = 0; i < node.getChildren().size(); ++i)
    {
        loadHFMesh(*renderable, hf, ret.getSkeleton(), *node.getChildren()[i], meshId, uvMaker.getUV());
    }
}

void GHHeightFieldNMeshLoader::loadHFMesh(GHGeometryRenderable& renderable, const GHHeightField& hf,
                                          GHTransformNode* skeleton, const GHXMLNode& node, int& meshIdx,
										  const float* uv) const
{
    GHHeightFieldMeshDesc desc;
    if (!desc.loadXML(node, mObjFactory)) {
        return;
    }
    
    char name[128];
    sprintf(name, "hfmesh%d", meshIdx);
    GHIdentifier meshId = mIdFactory.generateHash(name);
    
    GHTransformNode* meshRoot = GHHeightFieldMeshShared::createMeshNode(desc.getMeshOrigin(), meshId, hf, skeleton);
    
    GHPoint2i subPos(desc.getMeshPos());
    GHPoint2i subSize(desc.getMeshSize());
    subSize[0] /= desc.getDivisions()[0];
    subSize[1] /= desc.getDivisions()[1];
    
    GHPoint3 startMin(FLT_MAX, FLT_MAX, FLT_MAX);
    GHPoint3 startMax(FLT_MIN, FLT_MIN, FLT_MIN);
    
    for (int x = 0; x < desc.getDivisions()[0]; ++x)
    {
        subPos[1] = desc.getMeshPos()[1];
        for (int y = 0; y < desc.getDivisions()[1]; ++y)
        {
            GHPoint2i segSize(subSize);
            segSize += 1;
            
            GHRect<float, 3> bounds(startMin, startMax);
            GHVertexBuffer* hfVB = mMeshCreator.createVB(hf, subPos, segSize, desc.getMeshOrigin(), bounds, uv);
            if (!hfVB) {
                GHDebugMessage::outputString("Failed to create hfVB");
                return;
            }
            
            GHGeometry* hfGeo = new GHGeometry;
            renderable.addGeometry(hfGeo);
            hfGeo->setVB(new GHVertexBufferPtr(hfVB));
            hfGeo->setMaterial(desc.getMaterial());
            
            hfGeo->setId(meshId);
            hfGeo->setTransform(meshRoot);
            
            GHSphereBounds* sphereBounds = new GHSphereBounds;
            GHSphere sphere;
            sphere.fromAABB(bounds);
            sphereBounds->setSphere(sphere);
            hfGeo->setBounds(sphereBounds);
            
            meshIdx++;
            sprintf(name, "hfmesh%d", meshIdx);
            meshId = mIdFactory.generateHash(name);
            
            subPos[1] += subSize[1];
        }
        subPos[0] += subSize[0];
    }
}
