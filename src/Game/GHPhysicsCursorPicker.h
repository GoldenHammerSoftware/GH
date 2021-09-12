// Copyright Golden Hammer Software
#pragma once

#include "GHScreenPosUnprojector.h"

class GHInputState;
class GHViewInfo;
class GHPhysicsSim;
class GHPhysicsObject;

class GHPhysicsCursorPicker
{
public:
    GHPhysicsCursorPicker(const GHInputState& inputState,
                          const GHViewInfo& viewInfo,
                          GHPhysicsSim& physicsSim);
    
    void setDistance(float dist) { mDist = dist; }
    
    GHPhysicsObject* pickObject(int cursorIndex=0);
    
private:
    const GHInputState& mInputState;
    GHScreenPosUnprojector mUnprojector;
    GHPhysicsSim& mPhysicsSim;
    float mDist;
};
