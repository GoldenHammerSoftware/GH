// Copyright Golden Hammer Software
#pragma once

#include "GHMD3LoaderAnimModifier.h"
#include "GHString/GHIdentifier.h"

class GHTimeVal;

class GHMD3LoaderAnimModifierGPU : public GHMD3LoaderAnimModifier
{
public:
    GHMD3LoaderAnimModifierGPU(GHIdentifier interpProp, const GHTimeVal& time);
    
    virtual void modifyRootGeom(GHGeometry& geom, const GHMD3SurfaceDescription& surfaceDesc) const;
    virtual GHFrameAnimControllerVB* createAnimController(void) const;

	virtual GHVBDescription* createVBDescription(const GHMD3SurfaceDescription& surfaceDesc) const;
	virtual GHVBDescription* createAnimInfoVBDescription(const GHMD3SurfaceDescription& surfaceDesc) const;
    
private:
    GHIdentifier mInterpProp;
    const GHTimeVal& mTime;
};
