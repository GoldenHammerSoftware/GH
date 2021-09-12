// Copyright Golden Hammer Software
#pragma once

class GHAchievementNotifier
{
public:
    virtual ~GHAchievementNotifier(void) {}
    
    virtual void handleAchievementComplete(const char* achievementDesc) = 0;
};
