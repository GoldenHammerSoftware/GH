// Copyright Golden Hammer Software
#pragma once

#include "GHTransition.h"
#include <vector>

class GHController;
class GHControllerMgr;

// A transition that adds and removes controllers to a mgr on activate/deactivate
class GHControllerTransition : public GHTransition
{
public:
	GHControllerTransition(GHControllerMgr& controllerManager, GHController* controller, float priority=0);
	GHControllerTransition(GHControllerMgr& controllerManager, 
                           const std::vector<GHController*>& controllers, float priority=0);
	~GHControllerTransition(void);
	
	virtual void activate(void);
	virtual void deactivate(void);
	
private:
	GHControllerMgr& mControllerMgr;
    std::vector<GHController*> mControllers;
    float mPriority;
};
