// Copyright 2010 Golden Hammer Software
#include "GHRagdollTool.h"
#include "GHTransition.h"
#include "GHEntity.h"
#include "GHEntityHashes.h"
#include "GHModel.h"
#include "GHTransformNode.h"
#include "GHPhysicsObject.h"

GHRagdollTool::GHRagdollTool(GHTransition* transition,
                             GHEntity& entity,
                             GHRagdollGenerator* generator,
                             const GHTransform& spawnTransform)
: mTransition(transition)
, mEntity(entity)
, mGenerator(generator)
, mModel(*(GHModel*)entity.mProperties.getProperty(GHEntityHashes::MODEL))
, mSpawnTrans(spawnTransform)
{
    mGenerator->generateRagdoll();
}

GHRagdollTool::~GHRagdollTool(void)
{
    delete mTransition;
    delete mGenerator;
}

void GHRagdollTool::onActivate(void)
{
    mTransition->activate();
}

void GHRagdollTool::onDeactivate(void)
{
    mTransition->deactivate();
}

void GHRagdollTool::update(void)
{
    //mModel.getPhysicsObject()->setLinearVelocity(GHPoint3(0, 0, 0));
    //mModel.getPhysicsObject()->setAngularVelocity(GHPoint3(0,0,0));
}
