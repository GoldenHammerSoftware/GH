// Copyright Golden Hammer Software
#pragma once

#include "GHXMLObjLoader.h"

class GHXMLObjFactory;

class GHFontLoader : public GHXMLObjLoader
{
public:
    GHFontLoader(const GHXMLObjFactory& objFactory);
    virtual void* create(const GHXMLNode& node, GHPropertyContainer& extraData) const;
    virtual void populate(void* obj, const GHXMLNode& node, GHPropertyContainer& extraData) const;

private:
    const GHXMLObjFactory& mObjFactory;
};
