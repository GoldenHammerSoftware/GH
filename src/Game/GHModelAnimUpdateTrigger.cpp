// Copyright Golden Hammer Software
#include "GHModelAnimUpdateTrigger.h"
#include "GHModel.h"

GHModelAnimUpdateTrigger::GHModelAnimUpdateTrigger(GHModel& model)
: mModel(model)
{
}

void GHModelAnimUpdateTrigger::run(void)
{
    mModel.playRenderAnims();
}
