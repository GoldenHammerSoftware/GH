// Copyright Golden Hammer Software
#pragma once

#import "GHiOSViewMetal.h"
#include "GHRenderServices.h"

class GHMetalPipelineMgr;
class GHMetalShaderMgr;
class GHMetalShaderLibraryResourceLoader;

class GHMetalRenderServices : public GHRenderServices
{
public:
    GHMetalRenderServices(GHSystemServices& systemServices,
                          GHiOSViewMetal& view);
    ~GHMetalRenderServices(void);
    
    virtual void initAppShard(GHAppShard& appShard);
    
private:
    GHMetalPipelineMgr* mPipelineMgr{0};
    GHMetalShaderMgr* mShaderMgr{0};
    GHMetalShaderLibraryResourceLoader* mShaderLoader{0};
};
