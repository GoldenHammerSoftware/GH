// Copyright Golden Hammer Software
#pragma once

#include "GHPlatform/GHPrioritizedList.h"

class GHController;

class GHControllerMgr 
{
public:
	void update(void);
	void add(GHController* controller, float pri=0);
    void updatePriority(GHController* controller, float pri);
	void remove(GHController* controller);
	
private:
	GHPrioritizedList<GHController*> mControllers;
};
