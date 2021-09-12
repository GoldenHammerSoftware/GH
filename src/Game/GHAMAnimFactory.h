// Copyright Golden Hammer Software
#pragma once

#include <vector>
#include "GHString/GHIdentifier.h"
#include "GHVBDescription.h"
#include "GHAMStructs.h"

class GHFrameAnimControllerVB;
class GHTransformAnimController;
class GHSkinAnimController;
class GHSkeletonAnimController;
class GHTransformNode;

// factory class for creating the animation controllers.
class GHAMAnimFactory
{
public:
    virtual ~GHAMAnimFactory() {}
    
    virtual GHFrameAnimControllerVB* createMeshAnim(void) const = 0;
    virtual GHTransformAnimController* createNodeAnim(void) const = 0;
    virtual GHSkinAnimController* createSkinAnim(const std::vector<GHIdentifier>& nodeIds) const = 0;
    virtual GHSkeletonAnimController* createSkeletonAnim(GHTransformNode& skelTop) const = 0;;
    // perform any modifications necessary to a stream desc to fit with our flavor of animations.
    virtual void modifyStreamDesc(const GHAM::MeshHeader& meshHeader, GHVBDescription::StreamDescription& streamDesc) const = 0;
};
