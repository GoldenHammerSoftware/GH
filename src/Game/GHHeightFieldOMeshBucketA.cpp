// Copyright Golden Hammer Software
#include "GHHeightFieldOMeshBucketA.h"
#include "GHPlatform/GHDebugMessage.h"
#include "GHMath/GHIntersect.h"
#include "GHMath/GHRandom.h"
#include "GHHeightField.h"
#include "GHString/GHString.h"
#include "GHUtils/GHProfiler.h"
#include "GHHeightFieldOMeshVertChooser.h"
#include "GHMath/GHMath.h"
#include "GHTextureWorldMap.h"

using namespace GHHeightFieldOMesh;

GHHeightFieldOMeshBucketA::GHHeightFieldOMeshBucketA(const GHHeightFieldMeshDesc& desc, GHTransformNode& topNode,
                                                     const GHHeightField& hf, GHVBFactory& vbFactory,
                                                     const GHStringIdFactory& hashTable, const GHThreadFactory& threadFactory,
													 GHTextureWorldMap* weightMap)
: mTopNode(topNode)
, mDesc(desc)
, mHF(hf)
, mMeshCreator(mDesc, topNode, hf, vbFactory, hashTable)
, mVerts(0)
, mVertChooser(0)
{
	GHPoint3 color;
	GHPoint2 worldPos;

    mNumVerts = mDesc.getMeshSize()[0] * mDesc.getMeshSize()[1];
	mVerts = new GHHeightFieldOMesh::MeshVertList((unsigned int)mNumVerts);
    for (int x = 0; x < mDesc.getMeshSize()[0]; ++x)
    {
		for (int y = 0; y < mDesc.getMeshSize()[1]; ++y)
        {
            size_t vertIdx = x + y*mDesc.getMeshSize()[0];
            mVerts->mPos[vertIdx][0] = (float)(x + mDesc.getMeshPos()[0]);
			mVerts->mPos[vertIdx][1] = (float)(y + mDesc.getMeshPos()[1]);

			if (weightMap) 
			{
				worldPos[0] = (x + mDesc.getMeshPos()[0]) * hf.getDistBetweenNodes();
				worldPos[1] = (y + mDesc.getMeshPos()[1]) * hf.getDistBetweenNodes();
				weightMap->getPixel(worldPos, color);
				mVerts->mWeightMod[vertIdx] = 1.0f + color[0]*10.0f;
			}
			else {
				mVerts->mWeightMod[vertIdx] = 1.0f;
			}
        }
    }

    createStarterTriangles();
    
    mVertChooser = new GHHeightFieldOMeshVertChooser(*mVerts, mNumVerts, mTris, mHF, threadFactory, mDesc);
}

GHHeightFieldOMeshBucketA::~GHHeightFieldOMeshBucketA(void)
{
    for (size_t i = 0; i < mTris.size(); ++i) {
        delete mTris[i];
    }
    delete [] mVerts;
    delete mVertChooser;

	for (size_t i = 0; i < mWeightTris.size(); ++i) {
		delete mWeightTris[i];
	}
}

MeshTri** GHHeightFieldOMeshBucketA::createMeshTriPtr(void)
{
	MeshTri** ret = new MeshTri*;
	mWeightTris.push_back(ret);
	return ret;
}

void GHHeightFieldOMeshBucketA::createStarterTriangles(void)
{
    // create 2 starter triangles that encompass the entire region.
	MeshTri* t1 = new MeshTri(createMeshTriPtr());
    t1->mVerts[0] = 0;
    t1->mVerts[1] = mDesc.getMeshSize()[0]-1;
    t1->mVerts[2] = mDesc.getMeshSize()[0] * (mDesc.getMeshSize()[1]-1);
    t1->calcCircumCircle(*mVerts);
    mTris.push_back(t1);
    
	MeshTri* t2 = new MeshTri(createMeshTriPtr());
    t2->mVerts[0] = mDesc.getMeshSize()[0] * (mDesc.getMeshSize()[1]-1);
    t2->mVerts[1] = mDesc.getMeshSize()[0]-1;
    t2->mVerts[2] = mDesc.getMeshSize()[0] * mDesc.getMeshSize()[1] - 1;
	t2->calcCircumCircle(*mVerts);
    mTris.push_back(t2);
    
    t1->mEdges[1] = t2;
    t2->mEdges[0] = t1;
    
    mVerts->mActive[0] = true;
	mVerts->mActive[mDesc.getMeshSize()[0] - 1] = true;
	mVerts->mActive[mDesc.getMeshSize()[0] * (mDesc.getMeshSize()[1] - 1)] = true;
	mVerts->mActive[mDesc.getMeshSize()[0] * mDesc.getMeshSize()[1] - 1] = true;
}

void GHHeightFieldOMeshBucketA::generateTriangles(unsigned int targCount)
{
#define SPAMPROGRESS
#ifdef SPAMPROGRESS
    int outputCount = 0;
    int outputThresh = targCount/20;
#endif
    for (unsigned int i = 0; i < targCount; ++i)
    {
        unsigned int newVert = mVertChooser->pickVertToAdd();
        if (!newVert) {
            GHDebugMessage::outputString("Could not find a vert to add, aborting at %d", i);
            return;
        }
        addVert(newVert);
        
#ifdef SPAMPROGRESS
        outputCount++;
        if (outputCount > outputThresh) {
            GHDebugMessage::outputString("Added %d verts", outputCount);
            outputThresh += targCount/10;
        }
#endif
    }
}

void GHHeightFieldOMeshBucketA::addVert(unsigned int newVert)
{
    GHPROFILESCOPE("GHHeightFieldOMeshBucketA::addVert", GHString::CHT_REFERENCE)
    
    // flag so we don't add twice.
    if (mVerts->mActive[newVert] == true) return;
	mVerts->mActive[newVert] = true;
    
	std::vector<MeshTri*> intersectingTris;
	// find the triangles whose circumcircle intersects the vert.
	findCircumCircleIntersectTris(mVerts->mPos[newVert], intersectingTris);

	// find the edges that either point to nothing or point to live triangles.
    std::vector< std::pair<MeshTri*, size_t> > edges;
	findOrphanedEdges(intersectingTris, edges);

    // make new triangles by connecting those edges to our new point.
    std::vector<MeshTri*> newTris;
	connectEdgesToVert(newVert, edges, newTris);

    // connect the edges of our new triangles to each other.
	connectSharedTriangleEdges(newTris);

	for (size_t i = 0; i < mTris.size(); ++i) {
		MeshTri* currTri = mTris[i];
		if (!currTri->mActive) {
			continue;
		}
		for (int edgeId = 0; edgeId < 3; ++edgeId)
		{
			if (currTri->mEdges[edgeId] && !currTri->mEdges[edgeId]->mActive)
			{
				//GHDebugMessage::outputString("Dead edge %d detected on tri %d.", edgeId, i);
				// potentially avoid memory corruption.
				currTri->mEdges[edgeId] = 0;
			}
		}
	}

    // remove the dead triangles.
    for (size_t i = 0; i < intersectingTris.size(); ++i)
    {
        std::vector<MeshTri*>::iterator findIter;
        findIter = std::find(mTris.begin(), mTris.end(), intersectingTris[i]);
        assert(findIter != mTris.end());
        mTris.erase(findIter);
        delete intersectingTris[i];
    }
}

void GHHeightFieldOMeshBucketA::findCircumCircleIntersectTris(const GHHeightFieldOMesh::MeshVertList::VertPos& point, 
	std::vector<MeshTri*>& intersectingTris)
{
	// find all triangles that intersect newVert.
	for (size_t triIdx = 0; triIdx < mTris.size(); ++triIdx)
	{
		if (mTris[triIdx]->pointInCircumCircle(point)) {
			intersectingTris.push_back(mTris[triIdx]);
			mTris[triIdx]->mActive = false;
		}
	}
	if (!intersectingTris.size()) {
		GHDebugMessage::outputString("Tried to add a vert that intersects no tris");
	}
}

void GHHeightFieldOMeshBucketA::findOrphanedEdges(const std::vector<MeshTri*>& intersectingTris,
	std::vector< std::pair<MeshTri*, size_t> >& edges)
{
	for (size_t triIdx = 0; triIdx < intersectingTris.size(); ++triIdx)
	{
		bool foundEdge = false;
		for (size_t edgeIdx = 0; edgeIdx < 3; ++edgeIdx)
		{
			if (!intersectingTris[triIdx]->mEdges[edgeIdx] ||
				intersectingTris[triIdx]->mEdges[edgeIdx]->mActive)
			{
				edges.push_back(std::pair<MeshTri*, size_t>(intersectingTris[triIdx], edgeIdx));
				foundEdge = true;
			}
		}
		if (!foundEdge) {
			//GHDebugMessage::outputString("didn't find edge");
		}
	}
}

void GHHeightFieldOMeshBucketA::connectEdgesToVert(unsigned int newVert, 
	const std::vector< std::pair<MeshTri*, size_t> >& edges, std::vector<MeshTri*>& newTris)
{
	for (size_t i = 0; i < edges.size(); ++i)
	{
		unsigned int nv1 = 0;
		unsigned int nv2 = 0;
		if (edges[i].second == 0)
		{
			nv1 = edges[i].first->mVerts[0];
			nv2 = edges[i].first->mVerts[1];
		}
		else if (edges[i].second == 1)
		{
			nv1 = edges[i].first->mVerts[1];
			nv2 = edges[i].first->mVerts[2];
		}
		else
		{
			nv1 = edges[i].first->mVerts[2];
			nv2 = edges[i].first->mVerts[0];
		}
		//assert(nv1 && nv2);

		MeshTri* newTri = createNewTri(newVert, nv1, nv2);
		if (!newTri) continue;
		newTris.push_back(newTri);
		newTri->mEdges[1] = edges[i].first->mEdges[edges[i].second];

		// connect the old triangles to our new tri.
		for (size_t edgeIdx = 0; edgeIdx < 3; ++edgeIdx)
		{
			if (!newTri->mEdges[edgeIdx]) continue;
			unsigned int v1 = newTri->mVerts[edgeIdx];
			int v2Idx = (int)edgeIdx + 1;
			if (v2Idx >= 3) v2Idx = 0;
			unsigned int v2 = newTri->mVerts[v2Idx];
			MeshTri* otherTri = newTri->mEdges[edgeIdx];
			for (size_t testVIdx = 0; testVIdx < 3; ++testVIdx)
			{
				if (otherTri->mVerts[testVIdx] == v1)
				{
					int upTestVert = (int)testVIdx + 1;
					if (upTestVert >= 3) upTestVert = 0;
					int downTestVert = (int)testVIdx - 1;
					if (downTestVert < 0) downTestVert = 2;
					if (otherTri->mVerts[upTestVert] == v2)
					{
						otherTri->mEdges[testVIdx] = newTri;
					}
					else if (otherTri->mVerts[downTestVert] == v2)
					{
						otherTri->mEdges[downTestVert] = newTri;
					}
				}
			}
		}

		//assert(newTri->mVerts[0] && newTri->mVerts[1] && newTri->mVerts[2]);
	}
	if (!newTris.size()) {
		GHDebugMessage::outputString("Failed to add any tris for new vert");
	}
}

MeshTri* GHHeightFieldOMeshBucketA::createNewTri(unsigned int v0, unsigned int v1, unsigned int v2)
{
	// do not add the new triangle if it will be degenerate.
	// this is only likely to happen on the edge of the mesh.
	MeshVertList::VertPos colPos;
	GHIntersect::closestPointOnLine(mVerts->mPos[v0], mVerts->mPos[v1], mVerts->mPos[v2], colPos, 0);
	if (GHFloat::isEqual(colPos[0], mVerts->mPos[v0][0]) && GHFloat::isEqual(colPos[1], mVerts->mPos[v0][1]))
	{
		//GHDebugMessage::outputString("Filtered degenerate tri (%f,%f) (%f,%f) (%f,%f)",
		//                             nv1->mPos[0], nv1->mPos[1], nv2->mPos[0], nv2->mPos[1],
		//                             newVert.mPos[0], newVert.mPos[1]);
		return 0;
	}

	MeshTri* newTri = new MeshTri(createMeshTriPtr());
	mTris.push_back(newTri);
	newTri->mVerts[0] = v0;
	newTri->mVerts[1] = v1;
	newTri->mVerts[2] = v2;
	newTri->calcCircumCircle(*mVerts);
	return newTri;
}

void GHHeightFieldOMeshBucketA::connectSharedTriangleEdges(const std::vector<MeshTri*>& newTris)
{
	for (size_t currTriIdx = 0; currTriIdx < newTris.size(); ++currTriIdx)
	{
		MeshTri* currTri = newTris[currTriIdx];
		if (!currTri->mActive) continue;

		for (size_t testTriIdx = currTriIdx + 1; testTriIdx < newTris.size(); ++testTriIdx)
		{
			MeshTri* testTri = newTris[testTriIdx];
			if (!testTri->mActive) continue;
			if (testTri == currTri) continue;

			for (size_t vertIdx = 0; vertIdx < 3; ++vertIdx)
			{
				int upTestVert = (int)vertIdx + 1;
				if (upTestVert >= 3) upTestVert = 0;
				int downTestVert = (int)vertIdx - 1;
				if (downTestVert < 0) downTestVert = 2;

				if (testTri->mVerts[vertIdx] == currTri->mVerts[0])
				{
					if (testTri->mVerts[upTestVert] == currTri->mVerts[1])
					{
						currTri->mEdges[0] = testTri;
						testTri->mEdges[vertIdx] = currTri;
					}
					else if (testTri->mVerts[downTestVert] == currTri->mVerts[1])
					{
						currTri->mEdges[0] = testTri;
						testTri->mEdges[downTestVert] = currTri;
					}
				}
				else if (testTri->mVerts[vertIdx] == currTri->mVerts[1])
				{
					if (testTri->mVerts[upTestVert] == currTri->mVerts[2])
					{
						currTri->mEdges[1] = testTri;
						testTri->mEdges[vertIdx] = currTri;
					}
					else if (testTri->mVerts[downTestVert] == currTri->mVerts[2])
					{
						currTri->mEdges[1] = testTri;
						testTri->mEdges[downTestVert] = currTri;
					}
				}
				else if (testTri->mVerts[vertIdx] == currTri->mVerts[2])
				{
					if (testTri->mVerts[upTestVert] == currTri->mVerts[0])
					{
						currTri->mEdges[2] = testTri;
						testTri->mEdges[vertIdx] = currTri;
					}
					else if (testTri->mVerts[downTestVert] == currTri->mVerts[0])
					{
						currTri->mEdges[2] = testTri;
						testTri->mEdges[downTestVert] = currTri;
					}
				}
			}
		}
	}
}

void GHHeightFieldOMeshBucketA::createGeo(GHGeometryRenderable& renderable, int& meshIdx, const float* uv)
{
    std::vector<GHHeightFieldOMeshCreator::MeshTri> renderTris;
    renderTris.reserve(mTris.size());
    for (size_t triId = 0; triId < mTris.size(); ++triId) {
        GHHeightFieldOMeshCreator::MeshTri renderTri;
        for (size_t vertId = 0; vertId < 3; ++vertId)
        {
            renderTri.mVerts[vertId][0] = (int)mVerts->mPos[mTris[triId]->mVerts[vertId]][0];
            renderTri.mVerts[vertId][1] = (int)mVerts->mPos[mTris[triId]->mVerts[vertId]][1];
        }
        renderTris.push_back(renderTri);
    }
    mMeshCreator.createGeo(renderable, meshIdx, renderTris, uv);
}

void GHHeightFieldOMeshBucketA::debugPrint(void) const
{
    size_t numActiveVerts = 0;
    for (size_t i = 0; i < mNumVerts; ++i)
    {
        if (mVerts->mActive[i]) numActiveVerts++;
    }
    GHDebugMessage::outputString("NumVerts %d NumTris %d", numActiveVerts, mTris.size());
}

void GHHeightFieldOMeshBucketA::shareEdges(GHHeightFieldOMeshBucket& other)
{
    for (int x = 0; x < mDesc.getMeshSize()[0]; ++x)
    {
        // top edge
        if (mVerts->mActive[x]) {
            other.addVert((unsigned int)mVerts->mPos[x][0], (unsigned int)mVerts->mPos[x][1]);
        }
        // bottom edge
        size_t lowEdgeIdx = x + mDesc.getMeshSize()[0]*(mDesc.getMeshSize()[1]-1);
		if (mVerts->mActive[lowEdgeIdx]) {
            other.addVert((unsigned int)mVerts->mPos[lowEdgeIdx][0], (unsigned int)mVerts->mPos[lowEdgeIdx][1]);
        }
    }
    for (int y = 0; y < mDesc.getMeshSize()[1]; ++y)
    {
        size_t leftEdgeIdx = y*mDesc.getMeshSize()[0];
		if (mVerts->mActive[leftEdgeIdx]) {
			other.addVert((unsigned int)mVerts->mPos[leftEdgeIdx][0], (unsigned int)mVerts->mPos[leftEdgeIdx][1]);
        }
        size_t rightEdgeIdx = (y*mDesc.getMeshSize()[0]) + mDesc.getMeshSize()[0] - 1;
		if (mVerts->mActive[rightEdgeIdx]) {
			other.addVert((unsigned int)mVerts->mPos[rightEdgeIdx][0], (unsigned int)mVerts->mPos[rightEdgeIdx][1]);
        }
    }
}

void GHHeightFieldOMeshBucketA::addVert(unsigned int worldX, unsigned int worldY)
{
    int localX = worldX - mDesc.getMeshPos()[0];
    int localY = worldY - mDesc.getMeshPos()[1];
    if (localX < 0) return;
    if (localY < 0) return;
    if (localX >= mDesc.getMeshSize()[0]) return;
    if (localY >= mDesc.getMeshSize()[1]) return;

    size_t localIdx = localX + localY * mDesc.getMeshSize()[0];
    addVert((unsigned int)localIdx);
}

void GHHeightFieldOMeshBucketA::validateMesh(void)
{
	std::vector<unsigned int> tryAgainVerts;
	for (size_t vertIdx = 0; vertIdx < mNumVerts; ++vertIdx)
	{
		if (!mVerts->mActive[vertIdx]) {
			continue;
		}
		// find non-duplicated edges that aren't on a mesh border.
		// every non-border edge should be guaranteed to have 2 triangles.
		std::vector<unsigned int> nonDupEdges;
		findNonDupEdgesForVert((unsigned int)vertIdx, nonDupEdges);

		if (nonDupEdges.size())
		{
			//GHDebugMessage::outputString("Found edge that needs a triangle.");
			if (nonDupEdges.size() != 2) {
				GHDebugMessage::outputString("Bad triangle: was expecting 2 edges, got %d", nonDupEdges.size());
				// add it to a list for trying again.
				tryAgainVerts.push_back((unsigned int)vertIdx);
			}
			else 
			{
				// determine correct winding order.
				GHPoint2 edge1(mVerts->mPos[nonDupEdges[0]]);
				edge1 -= mVerts->mPos[vertIdx];
				GHPoint2 edge2(mVerts->mPos[nonDupEdges[1]]);
				edge2 -= mVerts->mPos[vertIdx];
				GHPoint3 edge1p3(edge1[0], edge1[1], 0.0f);
				GHPoint3 edge2p3(edge2[0], edge2[1], 0.0f);
				GHPoint3 cross;
				GHMath::cross(edge1p3, edge2p3, cross);
				if (cross[2] > 0)
				{
					createNewTri((unsigned int)vertIdx, nonDupEdges[0], nonDupEdges[1]);
				}
				else
				{
					createNewTri((unsigned int)vertIdx, nonDupEdges[1], nonDupEdges[0]);
				}
			}
		}
	}
	for (size_t vertIdx = 0; vertIdx < tryAgainVerts.size(); ++vertIdx)
	{
		std::vector<unsigned int> nonDupEdges;
		findNonDupEdgesForVert(tryAgainVerts[vertIdx], nonDupEdges);

		if (nonDupEdges.size())
		{
			if (nonDupEdges.size() != 2) {
				GHDebugMessage::outputString("Failed at second pass vert, got %d edges", nonDupEdges.size());
			}
			else {
				createNewTri(tryAgainVerts[vertIdx], nonDupEdges[0], nonDupEdges[1]);
			}
		}
	}
}

void GHHeightFieldOMeshBucketA::findNonDupEdgesForVert(unsigned int vert, std::vector<unsigned int>& nonDupEdges)
{
	// find all the edges associated with this vert.
	std::vector<unsigned int> edges;
	for (size_t triIdx = 0; triIdx < mTris.size(); ++triIdx)
	{
		MeshTri* currTri = mTris[triIdx];
		if (currTri->mVerts[0] == vert) {
			edges.push_back(currTri->mVerts[1]);
			edges.push_back(currTri->mVerts[2]);
		}
		else if (currTri->mVerts[1] == vert) {
			edges.push_back(currTri->mVerts[0]);
			edges.push_back(currTri->mVerts[2]);
		}
		else if (currTri->mVerts[2] == vert) {
			edges.push_back(currTri->mVerts[0]);
			edges.push_back(currTri->mVerts[1]);
		}
	}

	bool testOnEdge = false;
	if (mVerts->mPos[vert][0] == mDesc.getMeshPos()[0]) testOnEdge = true;
	else if (mVerts->mPos[vert][1] == mDesc.getMeshPos()[1]) testOnEdge = true;
	else if (mVerts->mPos[vert][0] == mDesc.getMeshPos()[0] + mDesc.getMeshSize()[0] - 1) testOnEdge = true;
	else if (mVerts->mPos[vert][1] == mDesc.getMeshPos()[1] + mDesc.getMeshSize()[1] - 1) testOnEdge = true;

	for (size_t edge1Idx = 0; edge1Idx < edges.size(); ++edge1Idx)
	{
		bool foundDup = false;
		for (size_t edge2Idx = 0; edge2Idx < edges.size(); ++edge2Idx)
		{
			if (edge1Idx == edge2Idx) continue;
			if (edges[edge1Idx] == edges[edge2Idx])
			{
				foundDup = true;
				break;
			}
		}
		if (!foundDup)
		{
			if (testOnEdge)
			{
				if (mVerts->mPos[edges[edge1Idx]][0] == mDesc.getMeshPos()[0]) continue;
				else if (mVerts->mPos[edges[edge1Idx]][1] == mDesc.getMeshPos()[1]) continue;
				else if (mVerts->mPos[edges[edge1Idx]][0] == mDesc.getMeshPos()[0] + mDesc.getMeshSize()[0] - 1) continue;
				else if (mVerts->mPos[edges[edge1Idx]][1] == mDesc.getMeshPos()[1] + mDesc.getMeshSize()[1] - 1) continue;
			}
			nonDupEdges.push_back(edges[edge1Idx]);
		}
	}
}
