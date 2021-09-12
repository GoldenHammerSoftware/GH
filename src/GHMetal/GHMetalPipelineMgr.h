#pragma once

#import <MetalKit/MetalKit.h>
#import <Metal/Metal.h>

#include "GHShaderType.h"
#include <map>

class GHMetalRenderDevice;
class GHMDesc;
class GHMetalVBBlitterIndex;

// Handles creating MTLRenderPipelineState.  We want as few of those as possible.
class GHMetalPipelineMgr
{
public:
    struct PipelineWrapper
    {
        id<MTLRenderPipelineState> mPipelineState{nil};
        MTLRenderPipelineReflection* mReflection{nil};
    };
    
public:
    GHMetalPipelineMgr(GHMetalRenderDevice& device);
    ~GHMetalPipelineMgr(void);

    const PipelineWrapper& getPipelineState(const GHMDesc& desc, GHMetalVBBlitterIndex* blitter, id<MTLFunction> __strong (&shaders)[ST_MAX]);
    
private:
    // turn a pipeline descriptor into a pipeline state.
    PipelineWrapper createPipelineState(MTLRenderPipelineDescriptor* desc);

    // create a new partially initialized pipeline state.
    MTLRenderPipelineDescriptor* getNewPipelineDescriptor(void);

    // get a minimal placeholder vertex description.
    MTLVertexDescriptor* getDefaultVertexDescriptor(void) { return mDefaultVertexDescriptor; }
    
private:
    GHMetalRenderDevice& mDevice;
    // Default shared minimal descriptor.
    MTLVertexDescriptor* mDefaultVertexDescriptor{nil};
    MTLRenderPipelineDescriptor* mPipelineDescriptor{nil};
    
    // id for caching PipelineWrappers which are expensive to construct.
    // needs to work with == and = and <.
    struct CacheIndex
    {
        // storing blitter instead of vertex description means we won't catch a lot of dupes.
        const GHMetalVBBlitterIndex* mBlitter{nullptr};
        // same with desc.
        const GHMDesc* mDesc{nullptr};
        // having these in our cache index makes them live forever.
        // we might need to fix bloat here.
        id<MTLFunction> mVertexShader{nil};
        id<MTLFunction> mPixelShader{nil};
        
        bool operator < (const CacheIndex& other) const
        {
            if (mBlitter != other.mBlitter)
            {
                if (mBlitter < other.mBlitter) return true;
                else return false;
            }
            if (mDesc != other.mDesc)
            {
                if (mDesc < other.mDesc) return true;
                else return false;
            }
            if (mVertexShader != other.mVertexShader)
            {
                if (mVertexShader < other.mVertexShader) return true;
                else return false;
            }
            if (mPixelShader != other.mPixelShader)
            {
                if (mPixelShader < other.mPixelShader) return true;
                else return false;
            }
            return false;
        }
    };
    std::map<CacheIndex, PipelineWrapper> mPipelineCache;
};

