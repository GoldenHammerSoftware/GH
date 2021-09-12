// Copyright Golden Hammer Software
#pragma once

#include "GHMaterialCallbackFactory.h"

class GHViewInfo;

// prefixStr is appended to each of the property names.
// example: prefixStr = Shadow, we look for ShadowViewProj.
class GHMaterialCallbackFactoryViewInfo : public GHMaterialCallbackFactory
{
public:
    GHMaterialCallbackFactoryViewInfo(const GHViewInfo& viewInfo,
                                      const char* prefixStr);
    
    virtual void createCallbacks(GHMaterial& mat) const;
    
private:
    const GHViewInfo& mViewInfo;
    
    static const int sMaxPropNameLength = 64;
    char mProjName[sMaxPropNameLength];
	char mProjInvName[sMaxPropNameLength];
    char mViewName[sMaxPropNameLength];
    char mViewProjName[sMaxPropNameLength];
    char mViewInvName[sMaxPropNameLength];
    char mEyeDirName[sMaxPropNameLength];
    char mEyePosName[sMaxPropNameLength];
};
