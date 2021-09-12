// Copyright Golden Hammer Software
#include "GHEntity.h"
#include "GHUtils/GHTransition.h"
#include <assert.h>

GHEntity::GHEntity(void)
{
}

GHEntity::~GHEntity(void)
{
    for (size_t i = 0; i < mExistenceTransitions.size(); ++i)
    {
        mExistenceTransitions[i]->deactivate();
        delete mExistenceTransitions[i];
    }
	mStateMachine.shutdown();
}

void GHEntity::addExistenceTransition(GHTransition* trans)
{
    assert(trans);
    mExistenceTransitions.push_back(trans);
    trans->activate();
}
