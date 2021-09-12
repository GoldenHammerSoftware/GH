// Copyright Golden Hammer Software
#pragma once

#include "GHMath/GHPoint.h"
#include "GHMath/GHRect.h"
#include "GHHeightFieldBlitterCache.h"

class GHVBFactory;
class GHVertexBuffer;
class GHHeightField;

// util class to create a vb from a height field.
class GHHeightFieldMeshCreator
{
public:
    GHHeightFieldMeshCreator(GHVBFactory& vbFactory);
    
    GHVertexBuffer* createVB(const GHHeightField& heightField, const GHPoint2i& meshPos,
                             const GHPoint2i& meshSize, const GHPoint2i& meshOrigin,
                             GHRect<float, 3>& bounds, const float* uv) const;
    
private:
    GHVBFactory& mVBFactory;
	mutable GHHeightFieldBlitterCache mBlitterCache;
};
