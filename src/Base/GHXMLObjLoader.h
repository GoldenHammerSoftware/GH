// Copyright Golden Hammer Software
#pragma once

#include <assert.h>
#define GH_NO_POPULATE virtual void populate(void* obj, const GHXMLNode& node, GHPropertyContainer& extraData) const { assert(false); }

class GHXMLNode;
class GHPropertyContainer;
class GHXMLObjFactory;

// interface for something that can take an xml node and turn it into something else.
class GHXMLObjLoader
{
public:
    virtual ~GHXMLObjLoader(void) {}

    // if we find an "inherit" attribute, then
    //  - load inherit from objFactory
    //  - call populate with results and current node.
    void* load(const GHXMLNode& node, GHPropertyContainer& extraData, const GHXMLObjFactory& objFactory) const;
	
	//optional method, but will need to override if used
	// return: whether anything was saved 
	virtual bool save(GHXMLNode& node, void* data, GHPropertyContainer* extraData) const;

    // create an object of the right type and populate() it with data
    virtual void* create(const GHXMLNode& node, GHPropertyContainer& extraData) const = 0;
    // expect obj to be the same type created in load()
    // fill in any extra data from the node.
    virtual void populate(void* obj, const GHXMLNode& node, GHPropertyContainer& extraData) const = 0;
};
