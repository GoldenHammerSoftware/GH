// Copyright Golden Hammer Software
#include "GHModelTransformAnimCloner.h"
#include "GHTransformAnimController.h"
#include "GHModel.h"
#include "GHPlatform/GHDebugMessage.h"

GHModelTransformAnimCloner::GHModelTransformAnimCloner(GHTransformAnimController& con)
: mCon(con)
{
}

void GHModelTransformAnimCloner::clone(const GHModel& srcModel, const std::vector<GHTransformNode*>& srcNodes,
                                       GHModel& cloneModel, std::vector<GHTransformNode*>& cloneNodes)
{
    for (size_t nodeIdx = 0; nodeIdx < srcNodes.size(); ++nodeIdx)
    {
        if (mCon.getTarget() == srcNodes[nodeIdx])
        {
            GHTransformAnimController* newAnim = (GHTransformAnimController*)mCon.clone();
            newAnim->setTarget(cloneNodes[nodeIdx]);
            cloneModel.addTransformAnimController(newAnim, new GHModelTransformAnimCloner(*newAnim));
            return;
        }
    }
    GHDebugMessage::outputString("GHModelTransformAnimCloner failed to find matching node");
}
