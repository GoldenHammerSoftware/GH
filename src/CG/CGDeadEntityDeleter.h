// Copyright 2010 Golden Hammer Software
#pragma once

#include "GHController.h"
#include <vector>

class GHEntity;

class CGDeadEntityDeleter : public GHController
{
public:
    CGDeadEntityDeleter(std::vector<GHEntity*>& entityList);
    
    virtual void update(void);
    virtual void onActivate(void);
    virtual void onDeactivate(void);
    
private:
    std::vector<GHEntity*>& mEntityList;
};
