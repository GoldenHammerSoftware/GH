// Copyright Golden Hammer Software
#pragma once

#include "GHMath/GHPoint.h"

// class to wrap a normalized grid of heights
class GHHeightField
{
public:
    GHHeightField(unsigned int nodeWidth, unsigned int nodeHeight, float distBetweenNodes);
    ~GHHeightField(void);
    
    void setHeight(unsigned int x, unsigned int y, float height);
    float getHeight(unsigned int x, unsigned int y) const;
    
    void getDimensions(unsigned int& nodeWidth, unsigned int& nodeHeight) const;
    float getDistBetweenNodes(void) const { return mDistBetweenNodes; }
    float* getHeights(void) const { return mHeights; }
    
private:
    unsigned int getNodeIdx(unsigned x, unsigned y) const;
    
private:
    float *mHeights;
    // number of nodes in a row
    unsigned int mNodeWidth;
    // number of columns of nodes.
    unsigned int mNodeHeight;
    // space between each node.
    float mDistBetweenNodes;
};
