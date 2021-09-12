// Copyright Golden Hammer Software
#pragma once

#include "GHString/GHIdentifier.h"
#include <vector>

class GHMaterial;
class GHGeometryRenderable;

// a class that loops through geometry and replaces the materials.
class GHMaterialReplacer
{
public:
    GHMaterialReplacer(void);
    ~GHMaterialReplacer(void);
    
    void setDefaultReplacement(GHMaterial* mat);
    void addReplacement(GHMaterial* mat, GHIdentifier targetId);
    void applyReplacements(GHGeometryRenderable& renderable);
    
private:
    struct Replacement
    {
        Replacement(void) : mMat(0), mTargetId(0) {}
        Replacement(GHMaterial* mat, GHIdentifier targetId) : mMat(mat), mTargetId(targetId) {}
        
        GHMaterial* mMat;
        GHIdentifier mTargetId;
    };
    GHMaterial* mDefaultReplacement;
    std::vector<Replacement> mReplacements;
};
