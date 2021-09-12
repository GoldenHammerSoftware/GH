// Copyright Golden Hammer Software
#include "GHEntitySpawnTransition.h"
#include "GHEntity.h"
#include "GHEntityHashes.h"
#include "GHModel.h"
#include "GHTransformNode.h"

GHEntitySpawnTransition::GHEntitySpawnTransition(GHEntity* entity, 
                                                 const GHIdentifier& activeState,
                                                 GHTransformNode* parentNode,
                                                 const GHTransform& offsetTransform)
: mOffsetTransform(offsetTransform)
, mEntity(entity)
, mActiveState(activeState)
, mParentNode(parentNode)
{
}

GHEntitySpawnTransition::~GHEntitySpawnTransition(void)
{
    delete mEntity;
}

void GHEntitySpawnTransition::activate(void)
{
    if (!mEntity) return;

    GHTransform spawnPos;
    calculateSpawnPos(spawnPos);

    GHModel* model = mEntity->mProperties.getProperty(GHEntityHashes::MODEL);
    if (model && model->getSkeleton()) {
        model->getSkeleton()->getLocalTransform() = spawnPos;
    }
    mEntity->mStateMachine.setState(mActiveState);
}

void GHEntitySpawnTransition::deactivate(void)
{
    if (!mEntity) return;
    mEntity->mStateMachine.setState(GHIdentifier(0));
}

void GHEntitySpawnTransition::calculateSpawnPos(GHTransform& outSpawnPos)
{
    outSpawnPos = mOffsetTransform;
    if (mParentNode) 
    {
        outSpawnPos.becomeWorldTransform(mParentNode->getTransform(), outSpawnPos);
    }
}
