// Copyright Golden Hammer Software
#pragma once

#include "GHXMLObjLoader.h"

class GHPropertyContainer;
class GHStringIdFactory;

/*
 <propCopy sourceId="blah" destId="blah"/>
*/
// we get the source container from extra data.
class GHPropertyCopyTransitionXMLLoader : public GHXMLObjLoader
{
public:
    GHPropertyCopyTransitionXMLLoader(GHPropertyContainer& destContainer, 
                                      const GHStringIdFactory& hashTable);
    
    virtual void* create(const GHXMLNode& node, GHPropertyContainer& extraData) const;
    GH_NO_POPULATE
    
private:
    GHPropertyContainer& mDestContainer;
    const GHStringIdFactory& mIdFactory;
};
