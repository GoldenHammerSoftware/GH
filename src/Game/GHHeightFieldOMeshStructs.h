// Copyright Golden Hammer Software
#pragma once

#include "GHMath/GHSphere.h"
#include "GHMath/GHPoint.h"
#include <vector>

// some useful structs for making optimized heightfield meshes.
namespace GHHeightFieldOMesh
{
    class MeshTri;
	class TriBucket;

	class MeshVertList
	{
	public:
		MeshVertList(unsigned int numVerts)
		{
			mPos = new VertPos[numVerts];
			mActive = new bool[numVerts];
			mWeight = new float[numVerts];
			mWeightTri = new MeshTri**[numVerts];
			mWeightMod = new float[numVerts];
			mTriBucket = new TriBucket*[numVerts];

			for (size_t i = 0; i < numVerts; ++i)
			{
				mActive[i] = false;
				mWeight[i] = 0;
				mWeightTri[i] = 0;
				mWeightMod[i] = 1.0f;
				mTriBucket[i] = 0;
			}
		}

		~MeshVertList(void)
		{
			delete[] mPos;
			delete[] mActive;
			delete[] mWeight;
			delete[] mWeightMod;
			delete[] mTriBucket;
			delete[] mWeightTri;
		}

	public:
		typedef GHPoint2 VertPos;
		// list of indicies into the height field.
		VertPos* mPos;
		// list to say which verts have been selected for the mesh.
		bool* mActive;
		// list of pointers to pointers to weight tris
		MeshTri*** mWeightTri;
		// list containing the calculated weights for the verts.
		// weight is only valid if mWeightTri is not null.
		float* mWeight;
		// list of modifiers to calculated weights, loaded from a texture.
		float* mWeightMod;
		// list of tri buckets associated with each vert.
		TriBucket** mTriBucket;
	};

    class MeshTri
    {
    public:
		MeshTri(MeshTri** weightPointer);
        ~MeshTri(void);
        
		void calcCircumCircle(const MeshVertList& verts);
        bool pointInCircumCircle(const MeshVertList::VertPos& pos) const;
        
    public:
        unsigned int mVerts[3];
        // the triangle connected to each edge.
        // can be 0 if on the edge of the map.
        // mEdges[0] is mVerts[0]->mVerts[1] edge
        // mEdges[1] is mVerts[1]->mVerts[2] edge
        // mEdges[2] is mVerts[2]->mVerts[0] edge
        MeshTri* mEdges[3];
        // the smallest circle that contains all 3 verts.
        GHSphere mCircumcircle;
        bool mActive;
		// a pointer to a pointer to this.  used to tell verts we don't exist anymore.
		MeshTri** mWeightPointer;
    };
    
    // a bucket for subdividing the triangles of the world.
    class TriBucket
    {
    public:
        TriBucket(const GHRect<float, 3>& bounds);
        
		bool containsPoint(const MeshVertList::VertPos& pos) const;
        const std::vector<MeshTri*>& getTris(void) const { return mTris; }
        
        void clearTris(void);
        void addTriIfContains(MeshTri* tri);
        
    private:
        GHRect<float, 3> mBounds;
        std::vector<MeshTri*> mTris;
    };
}
