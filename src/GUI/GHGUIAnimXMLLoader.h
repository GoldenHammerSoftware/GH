// Copyright Golden Hammer Software
#pragma once

#include "GHXMLObjLoader.h"

class GHPropertyContainer;
class GHStringIdFactory;
class GHGUIPosDescXMLLoader;

/*
 <guiAnim prop="GP_GUITRANS">
    <frame val=0>
        <guiPos align="0 0" offset="0 0" size="50 50"/>
    </frame>
    <frame val=1>
        <guiPos align="0.5 0.5" offset="0 0" size="50 50"/>
    </frame>
 </guiAnim>
 // the frames have to be in order from lowest to highest.
*/
class GHGUIAnimXMLLoader : public GHXMLObjLoader
{
public:
    GHGUIAnimXMLLoader(const GHPropertyContainer& props, const GHStringIdFactory& hashTable,
                       const GHGUIPosDescXMLLoader& posLoader);
    
    virtual void* create(const GHXMLNode& node, GHPropertyContainer& extraData) const;
    GH_NO_POPULATE
    
private:
    const GHPropertyContainer& mProps;
    const GHStringIdFactory& mIdFactory;
    const GHGUIPosDescXMLLoader& mPosLoader;
};
