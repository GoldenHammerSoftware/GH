// Copyright Golden Hammer Software
#include "GHHeightFieldOMeshStructs.h"
#include "GHMath/GHIntersect.h"
#include "GHPlatform/GHDebugMessage.h"

GHHeightFieldOMesh::MeshTri::MeshTri(MeshTri** weightPointer)
: mActive(true)
, mWeightPointer(weightPointer)
{
	*mWeightPointer = this;
    for (size_t i = 0; i < 3; ++i) {
        mVerts[i] = 0;
        mEdges[i] = 0;
    }
}

GHHeightFieldOMesh::MeshTri::~MeshTri(void)
{
    // unpoint the verts using this tri as weight.
	*mWeightPointer = 0;
}

void GHHeightFieldOMesh::MeshTri::calcCircumCircle(const MeshVertList& verts)
{
    float dummy;
    
    // for 2 edges find the perpindicular line that goes through the center of the edge.
    // find where those two lines intersect, that is our circle center.
    // the radius is the farthest distance from the corners.
	MeshVertList::VertPos edge0Diff = verts.mPos[mVerts[1]];
	edge0Diff -= verts.mPos[mVerts[0]];
    float edge0Len = edge0Diff.normalize();
    edge0Diff *= (edge0Len/2.0f);
	MeshVertList::VertPos edge0Center = verts.mPos[mVerts[0]];
    edge0Center += edge0Diff;
    dummy = edge0Diff[0];
    edge0Diff[0] = -1.0f * edge0Diff[1];
    edge0Diff[1] = dummy;
    
	MeshVertList::VertPos edge1Diff = verts.mPos[mVerts[2]];
	edge1Diff -= verts.mPos[mVerts[1]];
    float edge1Len = edge1Diff.normalize();
    edge1Diff *= (edge1Len/2.0f);
	MeshVertList::VertPos edge1Center = verts.mPos[mVerts[1]];
    edge1Center += edge1Diff;
    dummy = edge1Diff[0];
    edge1Diff[0] = -1.0f * edge1Diff[1];
    edge1Diff[1] = dummy;
    
    float edge0Dist = 0;
    float edge1Dist = 0;
    if (!GHIntersect::lineIntersect(edge0Center, edge0Diff, edge1Center, edge1Diff, edge0Dist, edge1Dist))
    {
        GHDebugMessage::outputString("Failed to intersect edges of triangle");
        // edges are probably co-linear.
        mCircumcircle.mRadius = 0;
        return;
    }
    
    // our triangle circumcircle center has now be found!
    GHPoint2 triCenter = edge0Center;
    edge0Diff *= edge0Dist;
    triCenter += edge0Diff;
    
    // find the radius by looking at the dist to a corner.
	GHPoint2 testPos = verts.mPos[mVerts[0]];
    testPos -= triCenter;
    float biggestDist = testPos.length();
    
    mCircumcircle.mCenter[0] = triCenter[0];
    mCircumcircle.mCenter[1] = triCenter[1];
    mCircumcircle.mCenter[2] = 0.0f;
    mCircumcircle.mRadius = biggestDist;
}

bool GHHeightFieldOMesh::MeshTri::pointInCircumCircle(const MeshVertList::VertPos& pos) const
{
	MeshVertList::VertPos diff = pos;
    diff[0] -= mCircumcircle.mCenter[0];
    diff[1] -= mCircumcircle.mCenter[1];
    float lenSqr = diff.lenSqr();
    if (lenSqr < mCircumcircle.mRadius*mCircumcircle.mRadius)
    {
        return true;
    }
    return false;
}

GHHeightFieldOMesh::TriBucket::TriBucket(const GHRect<float, 3>& bounds)
: mBounds(bounds)
{
}

bool GHHeightFieldOMesh::TriBucket::containsPoint(const MeshVertList::VertPos& pos) const
{
    return mBounds.containsPoint(pos);
}

void GHHeightFieldOMesh::TriBucket::clearTris(void)
{
    mTris.resize(0);
}

void GHHeightFieldOMesh::TriBucket::addTriIfContains(MeshTri* tri)
{
    if (GHIntersect::circleIntersectAABB(tri->mCircumcircle.mCenter, tri->mCircumcircle.mRadius, mBounds))
    {
        mTris.push_back(tri);
    }
}

