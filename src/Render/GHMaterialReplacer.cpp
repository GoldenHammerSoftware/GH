// Copyright Golden Hammer Software
#include "GHMaterialReplacer.h"
#include "GHMaterial.h"
#include "GHGeometryRenderable.h"
#include "GHGeometry.h"

GHMaterialReplacer::GHMaterialReplacer(void)
: mDefaultReplacement(0)
{
}

GHMaterialReplacer::~GHMaterialReplacer(void)
{
    for (size_t i = 0; i < mReplacements.size(); ++i)
    {
        mReplacements[i].mMat->release();
    }
    GHRefCounted::changePointer((GHRefCounted*&)mDefaultReplacement, 0);
}

void GHMaterialReplacer::setDefaultReplacement(GHMaterial* mat)
{
    GHRefCounted::changePointer((GHRefCounted*&)mDefaultReplacement, mat);
}

void GHMaterialReplacer::addReplacement(GHMaterial* mat, GHIdentifier targetId)
{
    mat->acquire();
    mReplacements.push_back(Replacement(mat, targetId));
}

void GHMaterialReplacer::applyReplacements(GHGeometryRenderable& renderable)
{
    const std::vector<GHGeometry*>& geometrySet = renderable.getGeometry();
    for (size_t i = 0; i < geometrySet.size(); ++i)
    {
        bool found = false;
        for (size_t repIdx = 0; repIdx < mReplacements.size(); ++repIdx)
        {
            if (geometrySet[i]->getId() == mReplacements[repIdx].mTargetId)
            {
                geometrySet[i]->setMaterial(mReplacements[repIdx].mMat);
                found = true;
                break;
            }
        }
        if (!found && mDefaultReplacement)
        {
            geometrySet[i]->setMaterial(mDefaultReplacement);
        }
    }
}

