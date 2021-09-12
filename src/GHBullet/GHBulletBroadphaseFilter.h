// Copyright Golden Hammer Software
#pragma once

#include "GHBulletBtInclude.h"

class GHBulletBroadphaseFilter : public btOverlapFilterCallback
{
public:
    virtual bool needBroadphaseCollision(btBroadphaseProxy* proxy0, btBroadphaseProxy* proxy1) const;
};
