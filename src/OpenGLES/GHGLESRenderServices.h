// Copyright 2010 Golden Hammer Software
#pragma once

#include "GHRenderServices.h"
#include "GHGLESShaderMgr.h"
#include "GHGLESStateCache.h"

class GHMutex;
class GHGLESTextureMgr;
class GHGLESContext;
class GHStringIdFactory;
class GHGLESReinitializer;

class GHGLESRenderServices : public GHRenderServices
{
public:
    GHGLESRenderServices(GHSystemServices& systemServices, 
                         GHMutex& renderMutex,
                         const GHStringIdFactory& hashTable);
    
    virtual void initAppShard(GHAppShard& appShard);

	GHGLESTextureMgr& getTextureMgr(void);
	GHGLESStateCache& getStateCache(void);

protected:
    // it's not convenient to pass the context as a construction argument.
    // init must be called by a subclass before this class can be used.
    void init(GHGLESContext& context);
    
	bool checkASTCSupport(void);
	bool checkETC2Support(void);

protected:
	GHGLESStateCache mStateCache;
    GHGLESShaderMgr mShaderMgr;
    GHMutex& mRenderMutex;
    GHGLESTextureMgr* mTextureMgr;
    GHGLESReinitializer* mReinitializer;
};
