// Copyright Golden Hammer Software
#pragma once

#include "GHUtils/GHTransition.h"
#include "GHString/GHIdentifier.h"
#include "GHMath/GHTransform.h"

class GHEntity;
class GHTransformNode;

// transition to spawn/remove an entity from the world
class GHEntitySpawnTransition : public GHTransition
{
public:
    GHEntitySpawnTransition(GHEntity* entity, 
                            const GHIdentifier& activeState,
                            GHTransformNode* parentNode,
                            const GHTransform& offsetTransform);
    ~GHEntitySpawnTransition(void);
    
    virtual void activate(void);
	virtual void deactivate(void);

private:
    void calculateSpawnPos(GHTransform& outSpawnPos);
    
private:
    GHTransform mOffsetTransform;
    GHEntity* mEntity;
    GHTransformNode* mParentNode;
    GHIdentifier mActiveState;
};
