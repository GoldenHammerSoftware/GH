#pragma once

class GHTexture;
class GHPropertyContainer;
class GHXMLNode;
class GHResourceFactory;

namespace GHGhcmXmlParser
{
    bool parseGHCM(GHResourceFactory& resourceFactory,
                          const GHXMLNode& node, GHPropertyContainer& extraData,
                          GHTexture* (&ret)[6]);
};
