// Copyright 2010 Golden Hammer Software
#pragma once
#ifndef GHMETAL

#import "GHiOSViewOgles.h"
#include "GHGLESRenderServices.h"

class GHiOSRenderServices : public GHGLESRenderServices
{
public:
    GHiOSRenderServices(GHSystemServices& systemServices, 
                        GHMutex& renderMutex,
                        GHiOSViewOgles& view,
                        bool allowMSAA,
                        const GHStringIdFactory& hashTable);
    
    virtual void initAppShard(GHAppShard& appShard);
};

#endif
