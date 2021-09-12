#pragma once

#include "GHUtils/GHResourceLoader.h"
#include "GHiOSFilenameCreator.h"
#include <vector>
#import <MetalKit/MetalKit.h>
#import <Metal/Metal.h>

class GHMetalRenderDevice;

class GHMetalTextureLoader : public GHResourceLoader
{
public:
    GHMetalTextureLoader(GHMetalRenderDevice& device);
    ~GHMetalTextureLoader(void);
    
    virtual GHResource* loadFile(const char* filename, GHPropertyContainer* extraData);
    
    void addOverrideLoader(GHResourceLoader* loader);
    
protected:
    GHMetalRenderDevice& mDevice;
    GHiOSFilenameCreator mFileNameCreator;
    std::vector<GHResourceLoader*> mOverrideLoaders;
    MTKTextureLoader* mMTKLoader{nil};
};
