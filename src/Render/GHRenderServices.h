// Copyright Golden Hammer Software
#pragma once

#include <vector>
#include "GHScreenInfo.h"

class GHRenderDevice;
class GHVBFactory;
class GHMaterialCallbackMgr;
class GHDeletionHandle;
class GHSystemServices;
class GHFontRenderer;
class GHRenderTargetFactory;
class GHAppShard;
class GHLightMgr;

// A central place to store render services for a particular platform.
//  We generally pass one of these to an app on construction.
class GHRenderServices 
{
public:
    GHRenderServices(GHSystemServices& systemServices);
    virtual ~GHRenderServices(void);
    
    virtual void initAppShard(GHAppShard& appShard);

    void setGraphicsQuality(int val);
    const int& getGraphicsQuality(void) { return mGraphicsQuality; }

    GHRenderDevice* getDevice(void) const { return mRenderDevice; }
    void setDevice(GHRenderDevice* device) { mRenderDevice = device; }
    
    GHVBFactory* getVBFactory(void) const { return mVBFactory; }
    void setVBFactory(GHVBFactory* factory) { mVBFactory = factory; }
    
    GHMaterialCallbackMgr* getMaterialCallbackMgr(void) const { return mMaterialCallbackMgr; }

    GHScreenInfo& getScreenInfo(void) { return mScreenInfo; }
    const GHScreenInfo& getScreenInfo(void) const { return mScreenInfo; }
    
    const GHFontRenderer* getFontRenderer(void) { return mFontRenderer; }
    const GHRenderTargetFactory* getRenderTargetFactory(void) { return mRenderTargetFactory; }
    
    GHLightMgr& getLightMgr(void) { return *mLightMgr; }
    
    void addOwnedItem(GHDeletionHandle* item) { mOwnedItems.push_back(item); }
    
protected:
    GHSystemServices& mSystemServices;
    GHScreenInfo mScreenInfo;
    GHMaterialCallbackMgr* mMaterialCallbackMgr;
    GHRenderDevice* mRenderDevice;
    GHVBFactory* mVBFactory;
    GHFontRenderer* mFontRenderer;
    GHRenderTargetFactory* mRenderTargetFactory;
    GHLightMgr* mLightMgr;
    int mGraphicsQuality;
    std::vector<GHDeletionHandle*> mOwnedItems;
};
