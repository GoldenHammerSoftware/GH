#include "GHGhcmXmlParser.h"
#include "GHXMLNode.h"
#include "GHResourceFactory.h"
#include "GHPlatform/GHDebugMessage.h"

bool GHGhcmXmlParser::parseGHCM(GHResourceFactory& resourceFactory, const GHXMLNode& node, GHPropertyContainer& extraData, GHTexture* (&ret)[6])
{
    const char* faceFilenames[6] = { 0 };
    const char* attrNames[6] = { "posx", "negx", "posy", "negy", "posz", "negz" };
    for (int i = 0; i < 6; ++i)
    {
        faceFilenames[i] = node.getAttribute(attrNames[i]);
        if (!faceFilenames[i])
        {
            GHDebugMessage::outputString("Error reading cubemap xml: missing %s attribute", attrNames[i]);
            return false;
        }
    }


    for (int i = 0; i < 6; ++i)
    {
        GHTexture* face = (GHTexture*)resourceFactory.getCacheResource(faceFilenames[i], &extraData);
        if (!face)
        {
            GHDebugMessage::outputString("Failed to load %s (%s cubemap face)", faceFilenames[i], attrNames[i]);
            return 0;
        }
        ret[i] = face;
    }

    return true;
}
