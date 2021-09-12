// Copyright 2010 Golden Hammer Software
#include "CGDeadEntityDeleter.h"
#include "GHEntity.h"
#include "CGProperties.h"

CGDeadEntityDeleter::CGDeadEntityDeleter(std::vector<GHEntity*>& entityList)
: mEntityList(entityList)
{
    
}

void CGDeadEntityDeleter::update(void)
{
    std::vector<GHEntity*>::iterator iter = mEntityList.begin();
    while (iter != mEntityList.end())
    {
        GHEntity* ent = *iter;
        if (!ent
            || ((int)ent->mProperties.getProperty(CGProperties::PP_HITPOINTS) <= 0
                && (bool)ent->mProperties.getProperty(CGProperties::PP_DIEIMMEDIATELY)))
        {
            delete ent;
            iter = mEntityList.erase(iter);
        }
        else 
        {
            ++iter;
        }
    }
}

void CGDeadEntityDeleter::onActivate(void)
{
    
}

void CGDeadEntityDeleter::onDeactivate(void)
{
    
}