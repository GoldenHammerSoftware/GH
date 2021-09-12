// Copyright Golden Hammer Software
#pragma once

#include "GHXMLObjLoader.h"

class GHStringIdFactory;

/*
<renderPassMembership passes="blah blah blah" excludedPasses="blah blah blah" emptySupportsAll=true/>
*/
class GHRenderPassMembershipXMLLoader : public GHXMLObjLoader
{
public:
    GHRenderPassMembershipXMLLoader(GHStringIdFactory& hashTable);
    
    virtual void* create(const GHXMLNode& node, GHPropertyContainer& extraData) const;
    virtual void populate(void* obj, const GHXMLNode& node, GHPropertyContainer& extraData) const;
    
private:
    GHStringIdFactory& mIdFactory;
};
