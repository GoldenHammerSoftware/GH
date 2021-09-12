// Copyright Golden Hammer Software
#pragma once

#include "GHMath/GHSphere.h"

class GHTransformNode;

// A class to map a sphere that follows a transform node around.
class GHSphereBounds 
{
public:
    GHSphereBounds(void);
    
    // make sure the world sphere is up to date and then return it.
    const GHSphere& getSphere(void) const;
    const GHSphere& getLocalSphere(void) const { return mLocalSphere; }
    // update our local-space sphere and set our mWordVersion to invalid.
    void setSphere(const GHSphere& sphere);
        
    void setTransform(GHTransformNode* node);
    const GHTransformNode* getTransform(void) const { return mTransform; }
    
private:
    // A transform that we follow around.
    GHTransformNode* mTransform;
    // We store a sphere in local space,
    GHSphere mLocalSphere;
    //  then we use a transform to calculate the sphere in world space.
    mutable GHSphere mWorldSphere;
    //  and we use the transform version to decide if we are up to date.
    mutable short mWorldVersion;
};
