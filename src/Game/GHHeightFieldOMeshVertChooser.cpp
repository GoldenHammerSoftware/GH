// Copyright Golden Hammer Software
#include "GHHeightFieldOMeshVertChooser.h"
#include "GHUtils/GHProfiler.h"
#include "GHHeightField.h"
#include "GHMath/GHIntersect.h"
#include "GHPlatform/GHDebugMessage.h"
#include "GHPlatform/GHThreadFactory.h"
#include "GHPlatform/GHThread.h"
#include "GHPlatform/GHMutex.h"
#include <float.h>
#include "GHHeightFieldMeshDesc.h"

GHHeightFieldOMeshVertChooser::GHHeightFieldOMeshVertChooser(MeshVertList& verts, size_t numVerts, const std::vector<MeshTri*>& tris,
                                                             const GHHeightField& hf, const GHThreadFactory& threadFactory,
                                                             const GHHeightFieldMeshDesc& desc)
: mTris(tris)
, mVerts(verts)
, mNumVerts(numVerts)
{
    // make an arbitrary number of workers subdividing the world.
    static const int sNumWorkers = 8;
	mVertWorkers.reserve(sNumWorkers);
    int numVertsPerWorker = (int)(numVerts/sNumWorkers);
    int vertStart = 0;
    for (size_t i = 0; i < sNumWorkers; ++i)
    {
        int vertEnd = vertStart+numVertsPerWorker;
        if (i == sNumWorkers) vertEnd = (int)numVerts;
        VertWorker* worker = new VertWorker(threadFactory, verts, mTriBuckets, vertStart, vertEnd, hf);
		mVertWorkers.push_back(worker);
        vertStart = vertEnd;

		TriBucketWorker* triWorker = new TriBucketWorker(threadFactory, mTris);
		mTriBucketWorkers.push_back(triWorker);
    }
    
    // make an arbitrary number of world subdivisions for triangles.
    static const int sNumWorldDivisions = 8;
    GHRect<float, 3> rect;
    rect.mMin[2] = -FLT_MAX;
    rect.mMax[2] = FLT_MAX;
    float xStart = (float)desc.getMeshPos()[0];
    GHPoint2 bucketSize;
    bucketSize[0] = (float)desc.getMeshSize()[0];
    bucketSize[1] = (float)desc.getMeshSize()[1];
    bucketSize /= (float)sNumWorldDivisions;
    for (size_t x = 0; x < sNumWorldDivisions; ++x)
    {
        rect.mMin[0] = xStart;
        xStart += bucketSize[0];
        rect.mMax[0] = xStart;
        
        float yStart = (float)desc.getMeshPos()[1];
        for (size_t y = 0; y < sNumWorldDivisions; ++y)
        {
            rect.mMin[1] = yStart;
            yStart += bucketSize[1];
            rect.mMax[1] = yStart;

            TriBucket* tb = new TriBucket(rect);
            mTriBuckets.push_back(tb);
        }
    }
}

GHHeightFieldOMeshVertChooser::~GHHeightFieldOMeshVertChooser(void)
{
	//GHDebugMessage::outputString("Deleting vworkers");
	for (size_t i = 0; i < mVertWorkers.size(); ++i)
    {
		delete mVertWorkers[i];
    }
	mVertWorkers.clear();
	//GHDebugMessage::outputString("Deleting triworkers");
	for (size_t i = 0; i < mTriBucketWorkers.size(); ++i)
	{
		delete mTriBucketWorkers[i];
	}
	mTriBucketWorkers.clear();
	//GHDebugMessage::outputString("Deleting tribuckets");
    for (size_t i = 0; i < mTriBuckets.size(); ++i)
    {
        delete mTriBuckets[i];
    }
	mTriBuckets.clear();
	//GHDebugMessage::outputString("done");
}

unsigned int GHHeightFieldOMeshVertChooser::pickVertToAdd(void)
{
	GHPROFILESCOPE("GHHeightFieldOMeshVertChooser::pickVertToAdd", GHString::CHT_REFERENCE)

	// update our tri buckets.
	size_t numBucketsSorted = 0;
	while (numBucketsSorted < mTriBuckets.size())
	{
		for (size_t i = 0; i < mTriBucketWorkers.size() && numBucketsSorted < mTriBuckets.size(); ++i)
		{
			mTriBucketWorkers[i]->setTriBucket(mTriBuckets[numBucketsSorted]);
			mTriBucketWorkers[i]->runAsync();
			numBucketsSorted++;
		}
		for (size_t i = 0; i < mTriBucketWorkers.size(); ++i)
		{
			// this looks like we are spinning, but
			//  once the worker starts, we end up waiting on its mutex.
			while (!mTriBucketWorkers[i]->isDone()) {}
		}
	}

    // launch our vert workers.
	for (size_t i = 0; i < mVertWorkers.size(); ++i) {
		mVertWorkers[i]->findBestVertAsync();
    }
    // wait for them to finish.
	for (size_t i = 0; i < mVertWorkers.size(); ++i) {
        // this looks like we are spinning, but
        //  once the worker starts, we end up waiting on its mutex.
		while (!mVertWorkers[i]->isDone()) {}
    }
    // find the best result from among the workers.
    float biggestDiff = 0;
    unsigned int ret = 0;
	for (size_t i = 0; i < mVertWorkers.size(); ++i)
    {
		if (biggestDiff < mVertWorkers[i]->getBestWeight()) {
			biggestDiff = mVertWorkers[i]->getBestWeight();
			ret = mVertWorkers[i]->getBestVert();
        }
    }
    
    return ret;
}

GHHeightFieldOMeshVertChooser::TriBucketWorker::TriBucketWorker(const GHThreadFactory& threadFactory, const std::vector<MeshTri*>& tris)
: mThread(threadFactory.createThread(GHThread::TP_HIGH))
, mMutex(threadFactory.createMutex())
, mTris(tris)
, mTriBucket(0)
, mIsDone(true)
{
}

GHHeightFieldOMeshVertChooser::TriBucketWorker::~TriBucketWorker(void)
{
	delete mThread;
	delete mMutex;
}

void GHHeightFieldOMeshVertChooser::TriBucketWorker::setTriBucket(TriBucket* bucket)
{
	mTriBucket = bucket;
}

void GHHeightFieldOMeshVertChooser::TriBucketWorker::run(void)
{
	if (!mTriBucket) {
		return;
	}

	mMutex->acquire();
	mIsDone = false;

	mTriBucket->clearTris();
	for (size_t triIdx = 0; triIdx < mTris.size(); ++triIdx)
	{
		mTriBucket->addTriIfContains(mTris[triIdx]);
	}

	mIsDone = true;
	mMutex->release();
}

bool GHHeightFieldOMeshVertChooser::TriBucketWorker::isDone(void)
{
	mMutex->acquire();
	bool ret = mIsDone;
	mMutex->release();
	return ret;
}

void GHHeightFieldOMeshVertChooser::TriBucketWorker::runAsync(void)
{
	mIsDone = false;
	mThread->runThread(*this);
}

GHHeightFieldOMeshVertChooser::VertWorker::VertWorker(const GHThreadFactory& threadFactory, MeshVertList& verts,
                                                      const std::vector<TriBucket*>& triBuckets,
                                                      size_t vertStart, size_t vertEnd, const GHHeightField& hf)
: mThread(threadFactory.createThread(GHThread::TP_HIGH))
, mMutex(threadFactory.createMutex())
, mVerts(verts)
, mTriBuckets(triBuckets)
, mVertStart(vertStart)
, mVertEnd(vertEnd)
, mHF(hf)
, mBestVert(0)
, mBestWeight(0)
, mIsDone(true)
{
}

GHHeightFieldOMeshVertChooser::VertWorker::~VertWorker(void)
{
    delete mThread;
    delete mMutex;
}

void GHHeightFieldOMeshVertChooser::VertWorker::findBestVertAsync(void)
{
    mIsDone = false;
    mThread->runThread(*this);
}

void GHHeightFieldOMeshVertChooser::VertWorker::run(void)
{
    mMutex->acquire();
    mIsDone = false;
    mBestVert = 0;
    mBestWeight = 0;

    GHPoint3 barycentricCoords;
    for (size_t vertIdx = mVertStart; vertIdx < mVertEnd; ++vertIdx)
    {
        if (mVerts.mActive[vertIdx]) continue;
        
		if (mVerts.mWeightTri[vertIdx] != 0 && *mVerts.mWeightTri[vertIdx] != 0)
		{
            // no need to recalculate.
            if (mVerts.mWeight[vertIdx] > mBestWeight)
            {
                mBestWeight = mVerts.mWeight[vertIdx];
                mBestVert = (unsigned int)vertIdx;
            }
            continue;
        }
        
//		float actualHeight = mHF.getHeight(mVerts.mPos[vertIdx][0], mVerts.mPos[vertIdx][1]);
        float heightDiff = 0;
        int numCol = 0;
        
        // find containing bucket
		if (!mVerts.mTriBucket[vertIdx])
		{
			for (size_t buckIdx = 0; buckIdx < mTriBuckets.size(); ++buckIdx)
			{
				if (mTriBuckets[buckIdx]->containsPoint(mVerts.mPos[vertIdx]))
				{
					mVerts.mTriBucket[vertIdx] = mTriBuckets[buckIdx];
				}
			}
		}
		if (!mVerts.mTriBucket[vertIdx]) {
			GHDebugMessage::outputString("Couldn't find bucket for node");
		}
		else
		{
			const std::vector<MeshTri*>& tris = mVerts.mTriBucket[vertIdx]->getTris();
			// find intersecting tri in bucket.
			for (size_t triIdx = 0; triIdx < tris.size(); ++triIdx)
			{
				MeshTri* currTri = tris[triIdx];
				if (currTri->pointInCircumCircle(mVerts.mPos[vertIdx]))
				{
					if (GHIntersect::pointInTriangle(mVerts.mPos[currTri->mVerts[0]], mVerts.mPos[currTri->mVerts[1]], mVerts.mPos[currTri->mVerts[2]],
						mVerts.mPos[vertIdx], barycentricCoords))
					{
						/*
						float testHeight = 0;
						for (int i = 0; i < 3; ++i) {
						testHeight += barycentricCoords[i]*mHF.getHeight(currTri->mVerts[i]->mPos[0], currTri->mVerts[i]->mPos[1]);
						}
						float testHeightDiff = ::fabs(actualHeight - testHeight);
						*/

						GHPoint3 p1(mVerts.mPos[currTri->mVerts[0]][0] * mHF.getDistBetweenNodes(),
							mVerts.mPos[currTri->mVerts[0]][1] * mHF.getDistBetweenNodes(),
							mHF.getHeight((unsigned int)mVerts.mPos[currTri->mVerts[0]][0], (unsigned int)mVerts.mPos[currTri->mVerts[0]][1]));
						GHPoint3 p2(mVerts.mPos[currTri->mVerts[1]][0] * mHF.getDistBetweenNodes(),
							mVerts.mPos[currTri->mVerts[1]][1] * mHF.getDistBetweenNodes(),
							mHF.getHeight((unsigned int)mVerts.mPos[currTri->mVerts[1]][0], (unsigned int)mVerts.mPos[currTri->mVerts[1]][1]));
						GHPoint3 p3(mVerts.mPos[currTri->mVerts[2]][0] * mHF.getDistBetweenNodes(),
							mVerts.mPos[currTri->mVerts[2]][1] * mHF.getDistBetweenNodes(),
							mHF.getHeight((unsigned int)mVerts.mPos[currTri->mVerts[2]][0], (unsigned int)mVerts.mPos[currTri->mVerts[2]][1]));
						GHPoint3 tp(mVerts.mPos[vertIdx][0] * mHF.getDistBetweenNodes(),
							mVerts.mPos[vertIdx][1] * mHF.getDistBetweenNodes(),
							mHF.getHeight((unsigned int)mVerts.mPos[vertIdx][0], (unsigned int)mVerts.mPos[vertIdx][1]));
						float testHeightDiff = GHIntersect::distFromTri(p1, p2, p3, tp);

						testHeightDiff *= mVerts.mWeightMod[vertIdx];

						if (testHeightDiff > heightDiff)
						{
							heightDiff = testHeightDiff;
							++numCol;
							mVerts.mWeight[vertIdx] = heightDiff;
							mVerts.mWeightTri[vertIdx] = currTri->mWeightPointer;
						}

						break;
					}
				}
			}
		}
        
        if (numCol > 0)
        {
            //GHDebugMessage::outputString("heightDiff %f", heightDiff);
            if (heightDiff > mBestWeight)
            {
                mBestWeight = heightDiff;
                mBestVert = (unsigned int)vertIdx;
            }
        }
        else
        {
            //GHDebugMessage::outputString("Found a vert that does not collide with any triangles");
        }
    }

    mIsDone = true;
    mMutex->release();
}

bool GHHeightFieldOMeshVertChooser::VertWorker::isDone(void)
{
    mMutex->acquire();
    bool ret = mIsDone;
    mMutex->release();
    return ret;
}
