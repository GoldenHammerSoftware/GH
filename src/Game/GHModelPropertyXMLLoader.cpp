// Copyright Golden Hammer Software
#include "GHModelPropertyXMLLoader.h"
#include "GHXMLNode.h"
#include "GHUtils/GHResourceFactory.h"
#include "GHModel.h"
#include "GHUtils/GHProperty.h"
#include "GHPlatform/GHDebugMessage.h"

GHModelPropertyXMLLoader::GHModelPropertyXMLLoader(GHResourceFactory& resourceCache)
: mResourceCache(resourceCache)
{
}

void* GHModelPropertyXMLLoader::create(const GHXMLNode& node, GHPropertyContainer& extraData) const
{
    GHModel* newModel = 0;
    const char* modelFileName = node.getAttribute("file");
    if (modelFileName)
    {
        GHModel* srcModel = (GHModel*)mResourceCache.getCacheResource(modelFileName);
        if (!srcModel) {
            GHDebugMessage::outputString("Failed to load srcModel for modelProp");
            return 0;
        }
        newModel = srcModel->clone();
    }
    if (!newModel) {
        GHDebugMessage::outputString("No model for modelProp");
        return 0;
    }
    GHProperty* ret = new GHProperty(newModel, newModel);
    return ret;
}
