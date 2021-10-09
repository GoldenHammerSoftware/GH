// Copyright Golden Hammer Software
#include "GHSkeletonDebugDrawer.h"
#include "Sim/GHTransformNode.h"
#include "GHDebugDraw.h"

GHSkeletonDebugDrawer::GHSkeletonDebugDrawer(GHTransformNode* skeleton, float lineLength, GHPoint3 color)
: mSkeleton(skeleton)
, mHalfLineLength(lineLength*.5f)
, mColor(color)
{
    
}

void GHSkeletonDebugDrawer::onActivate(void)
{
    addTransformNode(mSkeleton);
}

void GHSkeletonDebugDrawer::onDeactivate(void)
{
    size_t numIDs = mLineIds.size();
    for (size_t i = 0; i < numIDs; ++i)
    {
        GHDebugDraw::getSingleton().removeShape(mLineIds[i]);
    }
    mLineIds.resize(0);
}

void GHSkeletonDebugDrawer::update(void)
{
    mCurrentLineID = 0;
    updateTransformNode(mSkeleton);
}

void GHSkeletonDebugDrawer::addTransformNode(GHTransformNode* transformNode)
{
    GHPoint3 lhsPoint, rhsPoint;
    getHorizontalLine(transformNode, lhsPoint, rhsPoint);
    int id1 = GHDebugDraw::getSingleton().addLine(lhsPoint, rhsPoint, mColor);
    mLineIds.push_back(id1);
    
    getVerticalLine(transformNode, lhsPoint, rhsPoint);
    int id2 = GHDebugDraw::getSingleton().addLine(lhsPoint, rhsPoint, mColor);
    mLineIds.push_back(id2);

	for (size_t i = 0; i < transformNode->getChildren().size(); ++i)
    {
        addTransformNode(transformNode->getChildren()[i]);
    }
}

void GHSkeletonDebugDrawer::updateTransformNode(GHTransformNode* transformNode)
{
    GHPoint3 lhsPoint, rhsPoint;
    getHorizontalLine(transformNode, lhsPoint, rhsPoint);
    GHDebugDraw::getSingleton().setLine(mLineIds[mCurrentLineID], lhsPoint, rhsPoint, mColor);
    ++mCurrentLineID;
    getVerticalLine(transformNode, lhsPoint, rhsPoint);
    GHDebugDraw::getSingleton().setLine(mLineIds[mCurrentLineID], lhsPoint, rhsPoint, mColor);
    ++mCurrentLineID;
    
	for (size_t i = 0; i < transformNode->getChildren().size(); ++i)
    {
        updateTransformNode(transformNode->getChildren()[i]);
    }
}

void GHSkeletonDebugDrawer::getHorizontalLine(GHTransformNode* transformNode, GHPoint3& lhsPoint, GHPoint3& rhsPoint) const
{
    GHPoint3 pos;
    transformNode->getTransform().getTranslate(pos);
    lhsPoint.setCoords(pos[0]-mHalfLineLength, pos[1], pos[2]);
    rhsPoint.setCoords(pos[0]+mHalfLineLength, pos[1], pos[2]);
}

void GHSkeletonDebugDrawer::getVerticalLine(GHTransformNode* transformNode, GHPoint3& lhsPoint, GHPoint3& rhsPoint) const
{
    GHPoint3 pos;
    transformNode->getTransform().getTranslate(pos);
    lhsPoint.setCoords(pos[0], pos[1]-mHalfLineLength, pos[2]);
    rhsPoint.setCoords(pos[0], pos[1]+mHalfLineLength, pos[2]);
}
