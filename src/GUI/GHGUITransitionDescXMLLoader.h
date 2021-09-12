// Copyright Golden Hammer Software
#pragma once

#include "GHXMLObjLoader.h"

class GHGUITransitionDesc;
class GHStringIdFactory;

// populates a GHGUITransitionDesc from a node's attributes.
class GHGUITransitionDescXMLLoader : public GHXMLObjLoader
{
public:
    GHGUITransitionDescXMLLoader(const GHStringIdFactory& hashTable);
    
    virtual void* create(const GHXMLNode& node, GHPropertyContainer& extraData) const;
    virtual void populate(void* obj, const GHXMLNode& node, GHPropertyContainer& extraData) const;

private:
	const GHStringIdFactory& mIdFactory;
};
