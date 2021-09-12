// Copyright Golden Hammer Software
#include "GHInputStateClearer.h"
#include "GHInputState.h"

GHInputStateClearer::GHInputStateClearer(GHInputState& inputState)
: mInputState(inputState)
{
    
}

void GHInputStateClearer::update(void)
{
    mInputState.clearInputEvents();
}

void GHInputStateClearer::onActivate(void)
{
    
}

void GHInputStateClearer::onDeactivate(void)
{
    
}
