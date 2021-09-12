// Copyright 2010 Golden Hammer Software
#pragma once

#include "GHSystemServices.h"
#include "GHPrintfOutputPipe.h"

class GHEventMgr;
class GHOALSoundDevice;

class GHBBSystemServices : public GHSystemServices
{
public:
	GHBBSystemServices(void);
    
    virtual void initAppShard(GHAppShard& appShard);

private:
    GHPrintfOutputPipe mOutputPipe;

    //Stored as a pointer so that we can access OAL-specific function in initAppShard.
    // Ownership belongs to base class GHSystemServices, as mSoundDevice
    GHOALSoundDevice* mOALSoundDevice;
};
