// Copyright Golden Hammer Software
#include "GHControllerMgr.h"
#include "GHController.h"

void GHControllerMgr::update(void)
{
	mControllers.startIteration();
	size_t numControllers = mControllers.getNumEntries();
	for (size_t i = 0; i < numControllers; ++i)
	{
		GHController* currCon = mControllers.getEntry(i);
        if (!mControllers.isInChangeList(currCon))
        {
            currCon->update();
        }
	}
	mControllers.endIteration();
}

void GHControllerMgr::add(GHController* controller, float pri)
{
	mControllers.addEntry(controller, pri);
    controller->onActivate();
}

void GHControllerMgr::updatePriority(GHController* controller, float pri)
{
    mControllers.updatePriority(controller, pri);
}

void GHControllerMgr::remove(GHController* controller)
{
	mControllers.removeEntry(controller);
    controller->onDeactivate();
}

