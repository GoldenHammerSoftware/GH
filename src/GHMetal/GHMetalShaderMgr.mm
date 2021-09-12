#import "GHMetalShaderMgr.h"
#include "GHMetalRenderDevice.h"
#include "GHPlatform/GHDebugMessage.h"
#include "GHMetalShaderLibraryResourceLoader.h"
#include "GHMetalShaderLibraryResource.h"
#include "GHMetalNameSanitizer.h"

GHMetalShaderMgr::GHMetalShaderMgr(GHMetalRenderDevice& device, GHMetalShaderLibraryResourceLoader& shaderLoader)
: mDevice(device)
, mShaderLoader(shaderLoader)
{
    initDefaultShaders();
}

GHMetalShaderMgr::~GHMetalShaderMgr(void)
{
}

id<MTLFunction> GHMetalShaderMgr::getShaderFunction(const char* name)
{
    char strippedName[512];
    GHMetalNameSanitizer::sanitizeName(name, strippedName);
    NSString *shaderNameNS = [NSString stringWithUTF8String: strippedName];
    id<MTLFunction> ret = [mDefaultLibrary newFunctionWithName:shaderNameNS];
    if (ret)
    {
        return ret;
    }

    // fallback shaders.
    if (::strstr(name, "vertex"))
    {
        bool isGui = ::strstr(name, "gui");
        bool isFont = ::strstr(name, "font");
        if (isGui || isFont)
        {
            return mDefaultGuiVS;
        }
        return mDefaultVS;
    }
    return mDefaultPS;;
}

void GHMetalShaderMgr::initDefaultShaders(void)
{
    GHMetalShaderLibraryResource* res = (GHMetalShaderLibraryResource*)mShaderLoader.loadFile("GHDefaultShaders.metal");
    if (!res)
    {
        GHDebugMessage::outputString("Could not find default shaders");
        return;
    }
    __autoreleasing NSError *error = nil;
    mDefaultLibrary = res->mLibrary;
    mDefaultVS = [mDefaultLibrary newFunctionWithName:@"vertex_default"];
    mDefaultPS = [mDefaultLibrary newFunctionWithName:@"fragment_default"];
    mDefaultGuiVS = [mDefaultLibrary newFunctionWithName:@"vertex_gui_default"];
}
