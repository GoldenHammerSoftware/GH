// Copyright 2010 Golden Hammer Software
#pragma once
#include "GHGameServices.h"
#include "GHBBScoreloopScoresData.h"
#include "GHAchievementsData.h"

class GHBBGameServices : public GHGameServices
{
public:
    GHBBGameServices(GHSystemServices& systemServices,
    				GHRenderServices& renderServices,
    				GHMessageHandler& appMessageQueue);
    
    virtual void initAppShard(GHAppShard& appShard,
                              const GHTimeVal& time,
                              GHRenderGroup& renderGroup,
                              GHPhysicsSim& physicsSim);
    
public:
    GHBBScoreloopScoresData mScoreloopScoresData;
    GHAchievementsData mAchievementsData;
};
