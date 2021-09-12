// Copyright Golden Hammer Software
#pragma once

#include "GHMath/GHPoint.h"
#include <vector>

class GHHeightField;
class GHHeightFieldMeshDesc;
class GHVBFactory;
class GHTransformNode;
class GHStringIdFactory;
class GHGeometryRenderable;

// a class to take a set of precalculated triangles for an OMesh
// and turn it into a set of geometry
class GHHeightFieldOMeshCreator
{
public:
    typedef GHPoint2i VertPos;
    class MeshTri
    {
    public:
        // indicies into a height field for triangles.
        VertPos mVerts[3];
    };
    
public:
    GHHeightFieldOMeshCreator(const GHHeightFieldMeshDesc& desc, GHTransformNode& topNode,
                              const GHHeightField& hf, GHVBFactory& vbFactory,
                              const GHStringIdFactory& hashTable);
    
    void createGeo(GHGeometryRenderable& renderable, int& meshIdx,
		const std::vector<MeshTri>& tris, const float* uv) const;
    
private:
    void createGeoForArrays(std::vector<VertPos>& vertIds, std::vector<unsigned int>& indicies,
		GHGeometryRenderable& renderable, int meshIdx, const float* uv) const;
    
private:
    const GHHeightField& mHF;
    GHVBFactory& mVBFactory;
    const GHHeightFieldMeshDesc& mDesc;
    GHTransformNode& mTopNode;
    const GHStringIdFactory& mIdFactory;
};
