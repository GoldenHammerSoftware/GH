// Copyright Golden Hammer Software
#pragma once

#include "GHUtils/GHResourceLoader.h"

class GHXMLObjFactory;

// A resource loader for loading xml nodes into objects
//  by going through the xml obj factory.
// Should not be given xml nodes that don't represent GHResource objects.
class GHResourceLoaderXML : public GHResourceLoader
{
public:
    GHResourceLoaderXML(const GHXMLObjFactory& objFactory);
    
    virtual GHResource* loadFile(const char* filename, GHPropertyContainer* extraData);
    
private:
    const GHXMLObjFactory& mObjFactory;
};
