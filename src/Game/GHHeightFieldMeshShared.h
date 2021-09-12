// Copyright Golden Hammer Software
#pragma once

#include "GHMath/GHPoint.h"
#include "GHString/GHIdentifier.h"

class GHTransformNode;
class GHHeightField;

// dumping ground for shared functions between OMesh and NMesh
class GHHeightFieldMeshShared
{
public:
    // create a top node for a group of meshes based on an offset.
    // returns skeleton if there's no offset.
    static GHTransformNode* createMeshNode(const GHPoint2i& meshOrigin, const GHIdentifier& meshId,
                                           const GHHeightField& hf, GHTransformNode* skeleton);
};
