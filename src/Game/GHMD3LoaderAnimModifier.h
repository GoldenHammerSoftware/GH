// Copyright Golden Hammer Software
#pragma once

class GHFrameAnimControllerVB;
class GHGeometry;
struct GHMD3SurfaceDescription;
class GHVBDescription;

// interface to support loading md3s in different ways.
class GHMD3LoaderAnimModifier
{
public:
    virtual ~GHMD3LoaderAnimModifier(void) {}
    
    virtual void modifyRootGeom(GHGeometry& geom, const GHMD3SurfaceDescription& surfaceDesc) const = 0;
    virtual GHFrameAnimControllerVB* createAnimController(void) const = 0;

    // create the vb def used to create the main vb
	virtual GHVBDescription* createVBDescription(const GHMD3SurfaceDescription& surfaceDesc) const = 0;
    // create the vb def used to create vbs for anim frames.
	virtual GHVBDescription* createAnimInfoVBDescription(const GHMD3SurfaceDescription& surfaceDesc) const = 0;
};
