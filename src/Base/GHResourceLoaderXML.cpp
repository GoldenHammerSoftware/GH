// Copyright Golden Hammer Software
#include "GHResourceLoaderXML.h"
#include "GHXMLObjFactory.h"

GHResourceLoaderXML::GHResourceLoaderXML(const GHXMLObjFactory& objFactory)
: mObjFactory(objFactory)
{
}

GHResource* GHResourceLoaderXML::loadFile(const char* filename, GHPropertyContainer* extraData)
{
    return (GHResource*)mObjFactory.load(filename, extraData);
}

