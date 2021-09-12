// Copyright Golden Hammer Software
#pragma once

class GHGeometryRenderable;
class GHHeightFieldMeshDesc;

// a set of triangles for an optimized height field mesh
// all of these triangles get drawn with the same material and root node.
class GHHeightFieldOMeshBucket
{
public:
    virtual ~GHHeightFieldOMeshBucket(void) {}
    
	virtual const GHHeightFieldMeshDesc& getDesc(void) const = 0;

    // once we have all the triangles we want, turn them into geos.
	virtual void createGeo(GHGeometryRenderable& renderable, int& meshIdx, const float* uv) = 0;
    // create a set of triangles that matches our height field patch with targcount verts.
    virtual void generateTriangles(unsigned int targCount) = 0;
    virtual void debugPrint(void) const = 0;
    
    // we need adjacent buckets to have the same verts along the edge
    // call addVert on the neighbor with all our active edge verts.
    virtual void shareEdges(GHHeightFieldOMeshBucket& other) = 0;
    virtual void addVert(unsigned int worldX, unsigned int worldY) = 0;

	// go through the mesh and make sure there aren't any gaps.
	virtual void validateMesh(void) = 0;
};
