// Copyright Golden Hammer Software
#pragma once

#include "GHXMLObjLoader.h"

class GHControllerMgr;
class GHStringIdFactory;

// <skeletonDebugDrawer lineLength=0.02 color="1 0 0" targetNode=leftarm/>
class GHSkeletonDebugDrawerXMLLoader : public GHXMLObjLoader
{
public:
    GHSkeletonDebugDrawerXMLLoader(GHControllerMgr& controllerMgr, GHStringIdFactory& hashTable);
    
    virtual void* create(const GHXMLNode& node, GHPropertyContainer& extraData) const;
    GH_NO_POPULATE
    
private:
    GHControllerMgr& mControllerMgr;
    GHStringIdFactory& mIdFactory;
};
