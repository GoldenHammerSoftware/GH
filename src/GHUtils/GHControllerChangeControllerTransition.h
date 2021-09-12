// Copyright Golden Hammer Software
#pragma once

#include "GHUtils/GHTransition.h"
#include <vector>

class GHController;
class GHControllerChangeController;

// A transition that adds and removes controllers to a ControllerChangeController on activate/deactivate
class GHControllerChangeControllerTransition : public GHTransition
{
public:
	GHControllerChangeControllerTransition(GHControllerChangeController& controllerChanger, GHController* controller, float priority=0);
	GHControllerChangeControllerTransition(GHControllerChangeController& controllerChanger, 
                                           const std::vector<GHController*>& controllers, float priority=0);
	~GHControllerChangeControllerTransition(void);
	
	virtual void activate(void);
	virtual void deactivate(void);
	
private:
	GHControllerChangeController& mControllerChanger;
    std::vector<GHController*> mControllers;
    float mPriority;
};
