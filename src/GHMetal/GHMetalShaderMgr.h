#pragma once

#import <MetalKit/MetalKit.h>
#import <Metal/Metal.h>

class GHMetalRenderDevice;
class GHMetalShaderLibraryResourceLoader;

// Handles creating MTLRenderPipelineState.  We want as few of those as possible.
class GHMetalShaderMgr
{
public:
    GHMetalShaderMgr(GHMetalRenderDevice& device, GHMetalShaderLibraryResourceLoader& shaderLoader);
    ~GHMetalShaderMgr(void);
    
    // get a cache version of the shader for a string.
    id<MTLFunction> getShaderFunction(const char* name);
    
private:
    void initDefaultShaders(void);

private:
    GHMetalRenderDevice& mDevice;
    GHMetalShaderLibraryResourceLoader& mShaderLoader;
    
    id<MTLLibrary> mDefaultLibrary;
    id<MTLFunction> mDefaultVS;
    id<MTLFunction> mDefaultPS;
    id<MTLFunction> mDefaultGuiVS;
};

