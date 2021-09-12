// Copyright Golden Hammer Software
#pragma once

#include "GHHeightFieldOMeshStructs.h"
#include <vector>
#include "GHPlatform/GHRunnable.h"

using namespace GHHeightFieldOMesh;

class GHHeightField;
class GHThreadFactory;
class GHThread;
class GHMutex;
class GHHeightFieldMeshDesc;

// class to decide the next most important vert given references to the data.
class GHHeightFieldOMeshVertChooser
{
public:
    GHHeightFieldOMeshVertChooser(MeshVertList& verts, size_t numVerts, const std::vector<MeshTri*>& tris,
                                  const GHHeightField& hf, const GHThreadFactory& threadFactory,
                                  const GHHeightFieldMeshDesc& desc);
    ~GHHeightFieldOMeshVertChooser(void);
    
    unsigned int pickVertToAdd(void);

private:
    // looks at part of the list of verts and comes up with the most important.
    class VertWorker : public GHRunnable
    {
    public:
        VertWorker(const GHThreadFactory& threadFactory, MeshVertList& verts, const std::vector<TriBucket*>& triBuckets,
                   size_t vertStart, size_t vertEnd, const GHHeightField& hf);
        ~VertWorker(void);
        
        // tell our thread to call our run function.
        void findBestVertAsync(void);
        
        // the function called by our thread.
        virtual void run(void);
        
        float getBestWeight(void) const { return mBestWeight; }
        unsigned int getBestVert(void) const { return mBestVert; }
        bool isDone(void);
        
    private:
        MeshVertList& mVerts;
        const std::vector<TriBucket*>& mTriBuckets;
        size_t mVertStart;
        size_t mVertEnd;
        const GHHeightField& mHF;

        GHThread* mThread;
        GHMutex* mMutex;
        float mBestWeight;
        unsigned int mBestVert;
        bool mIsDone;
    };
	// looks at one tri bucket and decides which triangles fit inside.
	class TriBucketWorker : public GHRunnable
	{
	public:
		TriBucketWorker(const GHThreadFactory& threadFactory, const std::vector<MeshTri*>& tris);
		~TriBucketWorker(void);

		void runAsync(void);

		void setTriBucket(TriBucket* bucket);
		virtual void run(void);
		bool isDone(void);

	private:
		const std::vector<MeshTri*>& mTris;
		TriBucket* mTriBucket;
		GHThread* mThread;
		GHMutex* mMutex;
		bool mIsDone;
	};
    
private:
    const std::vector<MeshTri*>& mTris;
    std::vector<TriBucket*> mTriBuckets;
    std::vector<VertWorker*> mVertWorkers;
	std::vector<TriBucketWorker*> mTriBucketWorkers;
    
    MeshVertList& mVerts;
    size_t mNumVerts;
};
