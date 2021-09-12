#pragma once

#include "GHUtils/GHResourceLoader.h"

class GHFileOpener;
class GHMetalRenderDevice;
class GHMetalShaderLibraryResource;

class GHMetalShaderLibraryResourceLoader : public GHResourceLoader
{
public:
    GHMetalShaderLibraryResourceLoader(GHFileOpener& fileOpener, GHMetalRenderDevice& device);
    ~GHMetalShaderLibraryResourceLoader(void);
    
    virtual GHResource* loadFile(const char* filename, GHPropertyContainer* extraData=0) override;
    virtual GHResource* loadMemory(void* mem, size_t memSize, GHPropertyContainer* extraData = 0) override;
    
private:
    GHFileOpener& mFileOpener;
    GHMetalRenderDevice& mDevice;
    // For the shaders that are compiled with the binary.
    GHMetalShaderLibraryResource* mDefaultLibrary {0};
};

