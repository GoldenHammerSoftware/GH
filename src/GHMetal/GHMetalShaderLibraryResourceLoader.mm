#include "GHMetalShaderLibraryResourceLoader.h"
#include "GHPlatform/GHFileOpener.h"
#include "GHMetalRenderDevice.h"
#include "GHPlatform/GHDebugMessage.h"
#include "GHMetalShaderLibraryResource.h"

GHMetalShaderLibraryResourceLoader::GHMetalShaderLibraryResourceLoader(GHFileOpener& fileOpener, GHMetalRenderDevice& device)
: mFileOpener(fileOpener)
, mDevice(device)
{
    // Any shaders compiled with the program will be in the default library,
    //  and not as resource files.  If we fail to find any resource then
    //  fallback to the precompiled lib.
    __autoreleasing NSError *error = nil;
    id<MTLLibrary> defaultLibrary = [mDevice.getMetalDevice() newDefaultLibrary];
    if (!defaultLibrary)
    {
        GHDebugMessage::outputString("Failed to create default shader library");
    }
    else
    {
        mDefaultLibrary = new GHMetalShaderLibraryResource;
        mDefaultLibrary->acquire();
        mDefaultLibrary->mLibrary = defaultLibrary;
    }
}

GHMetalShaderLibraryResourceLoader::~GHMetalShaderLibraryResourceLoader(void)
{
    if (mDefaultLibrary) mDefaultLibrary->release();
}

GHResource* GHMetalShaderLibraryResourceLoader::loadFile(const char* filename, GHPropertyContainer* extraData)
{
    GHFile* file = mFileOpener.openFile(filename, GHFile::FT_TEXT, GHFile::FM_READONLY);
    if (!file)
    {
        // We didn't find it as a resource, so assume it was precompiled.
        return mDefaultLibrary;
    }
    
    // We did find it as a resource, so assume it is not precompiled.
    file->readIntoBuffer();
    char* buffer = 0;
    size_t bufferSize = 0;
    file->getFileBuffer(buffer, bufferSize);
    
    return loadMemory(buffer, bufferSize, extraData);
}

GHResource* GHMetalShaderLibraryResourceLoader::loadMemory(void* mem, size_t memSize, GHPropertyContainer* extraData)
{
    __autoreleasing NSError *error = nil;
    id<MTLLibrary> library;
    library = [mDevice.getMetalDevice() newLibraryWithSource:[NSString stringWithUTF8String:(const char*)mem] options:NULL error:&error];
    if (!library)
    {
        GHDebugMessage::outputString("Failed to create default library");
        return mDefaultLibrary;
    }
    
    GHMetalShaderLibraryResource* ret = new GHMetalShaderLibraryResource;
    ret->mLibrary = library;
    return ret;
}
