// Copyright Golden Hammer Software
#pragma once

#include "GHUtils/GHController.h"
#include <vector>
#include "GHMath/GHPoint.h"

class GHTransformNode;

//A utility class for putting debug draw crosshairs
// in all the bone positions in a GHTransformNode skeleton
class GHSkeletonDebugDrawer : public GHController
{
public:
    GHSkeletonDebugDrawer(GHTransformNode* skeleton, float lineLength=.02, GHPoint3 color=GHPoint3(0,0,1));

    virtual void onActivate(void);
    virtual void onDeactivate(void);
    virtual void update(void);
    
private:
    void addTransformNode(GHTransformNode* transformNode);
    void updateTransformNode(GHTransformNode* transformNode);
    void getHorizontalLine(GHTransformNode* transformNode, GHPoint3& lhsPoint, GHPoint3& rhsPoint) const;
    void getVerticalLine(GHTransformNode* transformNode, GHPoint3& lhsPoint, GHPoint3& rhsPoint) const;
    
private:
    float mHalfLineLength;
    GHPoint3 mColor;
    std::vector<int> mLineIds;
    int mCurrentLineID;
    GHTransformNode* mSkeleton;
};
