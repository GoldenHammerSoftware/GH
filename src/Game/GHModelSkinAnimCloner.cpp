// Copyright Golden Hammer Software
#include "GHModelSkinAnimCloner.h"
#include "GHSkinAnimController.h"
#include "GHModel.h"
#include "GHPlatform/GHDebugMessage.h"
#include "GHGeometryRenderable.h"
#include "GHGeometry.h"

GHModelSkinAnimCloner::GHModelSkinAnimCloner(GHSkinAnimController& con)
: mCon(con)
{
}

void GHModelSkinAnimCloner::clone(const GHModel& srcModel, const std::vector<GHTransformNode*>& srcNodes,
                                  GHModel& cloneModel, std::vector<GHTransformNode*>& cloneNodes)
{
    if (!srcModel.getRenderable() || !cloneModel.getRenderable()) {
        GHDebugMessage::outputString("GHModelSkinAnimCloner called with no renderable.");
        return;
    }
    
    int cloneGeoIdx = -1;
    for (size_t geoIdx = 0; geoIdx < srcModel.getRenderable()->getGeometry().size(); ++geoIdx)
    {
        if (srcModel.getRenderable()->getGeometry()[geoIdx]->getVB()->get() ==
            mCon.getTargetVB())
        {
            cloneGeoIdx = (int)geoIdx;
            break;
        }
    }
    if (cloneGeoIdx != -1)
    {
        GHSkinAnimController* cloneSkin = (GHSkinAnimController*)mCon.clone();
        cloneSkin->setTarget(cloneModel.getRenderable()->getGeometry()[cloneGeoIdx]->getVB()->get(),
                             cloneModel.getSkeleton());
        cloneModel.addAnimController(cloneSkin, new GHModelSkinAnimCloner(*cloneSkin));
    }
    else
    {
        GHDebugMessage::outputString("GHModelSkinAnimCloner failed to find matching node");
    }
}
