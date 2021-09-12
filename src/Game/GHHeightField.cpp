// Copyright Golden Hammer Software
#include "GHHeightField.h"
#include <assert.h>

GHHeightField::GHHeightField(unsigned int nodeWidth, unsigned int nodeHeight, float distBetweenNodes)
: mNodeWidth(nodeWidth)
, mNodeHeight(nodeHeight)
, mDistBetweenNodes(distBetweenNodes)
{
    mHeights = new float[nodeWidth*nodeHeight];
    for (unsigned int i = 0; i < nodeWidth*nodeHeight; ++i) {
        mHeights[i] = 0;
    }
}

GHHeightField::~GHHeightField(void)
{
    delete [] mHeights;
}

unsigned int GHHeightField::getNodeIdx(unsigned int x, unsigned int y) const
{
    if (x >= mNodeWidth) x = mNodeWidth-1;
    if (y >= mNodeHeight) y = mNodeHeight-1;
    unsigned int ret = x + y*mNodeWidth;
    assert(ret < mNodeWidth*mNodeHeight);
    return ret; 
}

void GHHeightField::setHeight(unsigned int x, unsigned int y, float height)
{
    int idx = getNodeIdx(x,y);
    mHeights[idx] = height;
}

float GHHeightField::getHeight(unsigned int x, unsigned int y) const
{
    int idx = getNodeIdx(x,y);
    return mHeights[idx];
}

void GHHeightField::getDimensions(unsigned int& nodeWidth, unsigned int& nodeHeight) const
{
    nodeWidth = mNodeWidth;
    nodeHeight = mNodeHeight;
}
