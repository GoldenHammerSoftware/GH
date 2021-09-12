// Copyright Golden Hammer Software
#pragma once

class GHGeometryRenderable;
class GHVertexBuffer;
class GHGeometry;
class GHVBBlitterIndex;

// comparison class to see if two geometries are the same
class GHGeometryComparison
{
public:
    static bool compareGeometries(GHGeometryRenderable& left, GHGeometryRenderable& right);
    static bool compareGeo(GHGeometry& srcGeo, GHGeometry& destGeo);
    static bool compareIB(GHVBBlitterIndex& srcIndex, GHVBBlitterIndex& destIndex);
    static bool compareStreams(GHVertexBuffer& srcBuf, GHVertexBuffer& destBuf);
};

