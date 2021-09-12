// Copyright Golden Hammer Software
#pragma once

#include "GHXMLObjLoader.h"

class GHStringIdFactory;
class GHPropertyContainer;

/*
 <propStringGenerator text="^999%d0tris">
    <monitorProperties>
        <GP_NUMTRIS/>
    </monitorProperties>
 </propStringGenerator> 
*/
class GHPropertyStringGeneratorXMLLoader : public GHXMLObjLoader
{
public:
    GHPropertyStringGeneratorXMLLoader(const GHPropertyContainer& propertyContainer,
                                       const GHStringIdFactory& hashTable);
    
    virtual void* create(const GHXMLNode& node, GHPropertyContainer& extraData) const;
    GH_NO_POPULATE

private:
    const GHPropertyContainer& mPropertyContainer;
    const GHStringIdFactory& mIdFactory;
};
