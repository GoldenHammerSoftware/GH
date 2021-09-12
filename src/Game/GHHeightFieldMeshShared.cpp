// Copyright Golden Hammer Software
#include "GHHeightFieldMeshShared.h"
#include "GHTransformNode.h"
#include "GHHeightField.h"

GHTransformNode* GHHeightFieldMeshShared::createMeshNode(const GHPoint2i& meshOrigin, const GHIdentifier& meshId,
                                                         const GHHeightField& hf, GHTransformNode* skeleton)
{
    GHTransformNode* meshRoot = 0;
    
    if (meshOrigin[0] == 0 && meshOrigin[1] == 0) {
        meshRoot = skeleton;
    }
    else {
        meshRoot = new GHTransformNode;
        meshRoot->getLocalTransform().becomeIdentity();
        GHPoint3 meshNodePos(hf.getDistBetweenNodes()*(float)meshOrigin[0], 0,
                             hf.getDistBetweenNodes()*(float)meshOrigin[1]);
        meshRoot->getLocalTransform().setTranslate(meshNodePos);
        meshRoot->setId(meshId);
        skeleton->addChild(meshRoot);
    }
    
    return meshRoot;
}
