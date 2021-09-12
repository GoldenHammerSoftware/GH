// Copyright Golden Hammer Software
#pragma once

#include "GHHeightFieldMeshCreator.h"
#include "GHString/GHIdentifierMap.h"

class GHModel;
class GHXMLNode;
class GHGeometryRenderable;
class GHHeightField;
class GHTransformNode;
class GHXMLObjFactory;
class GHStringIdFactory;

// class to load normalized grid meshes for a heightfield.
class GHHeightFieldNMeshLoader
{
public:
    GHHeightFieldNMeshLoader(GHXMLObjFactory& objFactory, const GHStringIdFactory& hashTable, 
		GHVBFactory& vbFactory, const GHIdentifierMap<int>& enumMap);
    
    void createRenderable(GHModel& ret, const GHHeightField& hf, const GHXMLNode& node) const;
    
private:
    void loadHFMesh(GHGeometryRenderable& renderable, const GHHeightField& hf,
                    GHTransformNode* skeleton, const GHXMLNode& node, int& meshIdx,
					const float* uv) const;
    
private:
    GHXMLObjFactory& mObjFactory;
    const GHStringIdFactory& mIdFactory;
	const GHIdentifierMap<int>& mEnumMap;
    GHHeightFieldMeshCreator mMeshCreator;
};
