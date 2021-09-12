// Copyright Golden Hammer Software
#include "GHModelFrameAnimVBAnimCloner.h"
#include "GHFrameAnimControllerVB.h"
#include "GHModel.h"
#include "GHPlatform/GHDebugMessage.h"
#include "GHGeometryRenderable.h"

GHModelFrameAnimVBAnimCloner::GHModelFrameAnimVBAnimCloner(GHFrameAnimControllerVB& con)
: mCon(con)
{
}

void GHModelFrameAnimVBAnimCloner::clone(const GHModel& srcModel, const std::vector<GHTransformNode*>& srcNodes,
                                         GHModel& cloneModel, std::vector<GHTransformNode*>& cloneNodes)
{
    if (!srcModel.getRenderable() || !cloneModel.getRenderable()) {
        GHDebugMessage::outputString("GHModelFrameAnimVBAnimCloner called with no renderable.");
        return;
    }
    
    for (size_t geoIdx = 0; geoIdx < srcModel.getRenderable()->getGeometry().size(); ++geoIdx)
    {
        if (mCon.getTarget() == srcModel.getRenderable()->getGeometry()[geoIdx])
        {
            GHFrameAnimControllerVB* newAnim = (GHFrameAnimControllerVB*)mCon.clone();
            newAnim->setTarget(cloneModel.getRenderable()->getGeometry()[geoIdx]);
            cloneModel.addGeometryAnimController(newAnim, new GHModelFrameAnimVBAnimCloner(*newAnim));
            return;
        }
    }
    GHDebugMessage::outputString("GHModelFrameAnimVBAnimCloner failed to find matching node");
}
