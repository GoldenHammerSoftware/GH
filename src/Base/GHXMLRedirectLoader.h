// Copyright Golden Hammer Software
#pragma once

#include "GHXMLObjLoader.h"

class GHXMLObjFactory;

/* <redirect file="blah.xml"/> */
// a class to stick a node from file into another node
class GHXMLRedirectLoader : public GHXMLObjLoader
{
public:
    GHXMLRedirectLoader(const GHXMLObjFactory& objFactory);
    virtual void* create(const GHXMLNode& node, GHPropertyContainer& extraData) const;
    
    virtual void populate(void* obj, const GHXMLNode& node, GHPropertyContainer& extraData) const {
        assert(false);
    }

private:
    const GHXMLObjFactory& mObjFactory;
};
