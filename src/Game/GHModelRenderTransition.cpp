// Copyright Golden Hammer Software
#include "GHModelRenderTransition.h"
#include "GHModel.h"

GHModelRenderTransition::GHModelRenderTransition(GHModel& model, 
                                                 GHRenderGroup& renderGroup,
                                                 const GHIdentifier& renderType)
: mModel(model)
, mRenderGroup(renderGroup)
, mRenderType(renderType)
{
}

void GHModelRenderTransition::activate(void)
{
    mModel.addRenderableToScene(mRenderGroup, mRenderType);
}

void GHModelRenderTransition::deactivate(void)
{
    mModel.removeRenderableFromScene(mRenderGroup, mRenderType);
}
