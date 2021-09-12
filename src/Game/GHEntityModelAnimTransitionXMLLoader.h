// Copyright Golden Hammer Software
#pragma once

#include "GHXMLObjLoader.h"

class GHStringIdFactory;
class GHTimeVal;

/*
<entityAnimTransition anim="idle run" randTimeOffset=0/>
*/
class GHEntityModelAnimTransitionXMLLoader : public GHXMLObjLoader
{
public:
    GHEntityModelAnimTransitionXMLLoader(const GHStringIdFactory& hashTable, const GHTimeVal& time);
    
    virtual void* create(const GHXMLNode& node, GHPropertyContainer& extraData) const;
    GH_NO_POPULATE
    
private:
    const GHStringIdFactory& mIdFactory;
    const GHTimeVal& mTime;
};
