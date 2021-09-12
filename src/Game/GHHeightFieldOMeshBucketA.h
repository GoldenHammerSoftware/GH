// Copyright Golden Hammer Software
#pragma once

#include "GHHeightFieldOMeshBucket.h"
#include "GHHeightFieldOMeshCreator.h"
#include "GHHeightFieldMeshDesc.h"
#include "GHMath/GHSphere.h"
#include "GHHeightFieldOMeshStructs.h"

using namespace GHHeightFieldOMesh;

class GHHeightField;
class GHTransformNode;
class GHHeightFieldOMeshVertChooser;
class GHThreadFactory;
class GHTextureWorldMap;

// uses the add new vert and split triangle method to create optimized mesh
// uses the Bowyer-Watson algorithm to add new verts.
class GHHeightFieldOMeshBucketA : public GHHeightFieldOMeshBucket
{
public:
    GHHeightFieldOMeshBucketA(const GHHeightFieldMeshDesc& desc, GHTransformNode& topNode,
                              const GHHeightField& hf, GHVBFactory& vbFactory,
                              const GHStringIdFactory& hashTable, const GHThreadFactory& threadFactory,
							  GHTextureWorldMap* weightMap);
    virtual ~GHHeightFieldOMeshBucketA(void);
    
	virtual const GHHeightFieldMeshDesc& getDesc(void) const { return mDesc; }

    // create a set of triangles that matches our height field patch with targcount verts.
    virtual void generateTriangles(unsigned int targCount);
    // once we have all the triangles we want, turn them into geos.
	virtual void createGeo(GHGeometryRenderable& renderable, int& meshIdx, const float* uv);
    virtual void debugPrint(void) const;
    
    // we need adjacent buckets to have the same verts along the edge
    // call addVert on the neighbor with all our active edge verts.
    virtual void shareEdges(GHHeightFieldOMeshBucket& other);
    // if this vert is within our region then activate it.
    virtual void addVert(unsigned int worldX, unsigned int worldY);

	// go through the mesh and make sure there aren't any gaps.
	virtual void validateMesh(void);

private:
    void createStarterTriangles(void);
	void addVert(unsigned int newVert);
	void findCircumCircleIntersectTris(const GHHeightFieldOMesh::MeshVertList::VertPos& point, std::vector<MeshTri*>& intersectingTris);
	void findOrphanedEdges(const std::vector<MeshTri*>& intersectingTris, 
		std::vector< std::pair<MeshTri*, size_t> >& edges);
	void connectEdgesToVert(unsigned int newVert,
		const std::vector< std::pair<MeshTri*, size_t> >& edges, std::vector<MeshTri*>& newTris);
	void connectSharedTriangleEdges(const std::vector<MeshTri*>& newTris);
	MeshTri* createNewTri(unsigned int v0, unsigned int v1, unsigned int v2);

	void findNonDupEdgesForVert(unsigned int vert, std::vector<unsigned int>& nonDupEdges);

	MeshTri** createMeshTriPtr(void);

private:
    const GHHeightField& mHF;
    GHHeightFieldMeshDesc mDesc;
    GHTransformNode& mTopNode;
    GHHeightFieldOMeshCreator mMeshCreator;
    GHHeightFieldOMeshVertChooser* mVertChooser;
    
	MeshVertList* mVerts;
    size_t mNumVerts;
    std::vector<MeshTri*> mTris;

	// pointer pointers to tris.  gets bigger every time we create a new tri.
	std::vector<MeshTri**> mWeightTris;
};
