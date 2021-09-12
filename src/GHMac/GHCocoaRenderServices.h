// Copyright 2010 Golden Hammer Software
#pragma once

#include "GHRenderServices.h"
#include "GHCocoaView.h"

class GHSystemServices;
class GHMutex;
class GHOGLTextureMgr;
class GHMutex;

class GHCocoaRenderServices : public GHRenderServices
{
public:
    GHCocoaRenderServices(GHSystemServices& systemServices, 
                          GHMutex& renderMutex,
                          GHCocoaView& cocoaView);
    
    virtual void initAppShard(GHAppShard& appShard);
    
private:
    GHOGLTextureMgr* mTextureMgr;
    GHMutex& mRenderMutex;
};
