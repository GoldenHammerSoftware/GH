// Copyright 2010 Golden Hammer Software
#pragma once

#include "GHMath/GHPoint.h"

class GHVBBlitterIndex;
class GHVertexStream;

// Sorts the triangles in an index buffer according to distance from a point.
class GHIndexBufferSorter
{
public:
	void sortIB(GHVBBlitterIndex& ib, GHVertexStream& posStream, const GHPoint3& targetPoint);
};
