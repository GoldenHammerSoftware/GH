// Copyright Golden Hammer Software
#pragma once

#include "GHXMLObjLoader.h"
#include "GHString/GHIdentifier.h"

class GHGUIButtonXMLLoader;
class GHMessageHandler;

class GHHouseAdXMLLoader : public GHXMLObjLoader
{
public:
    GHHouseAdXMLLoader(const GHGUIButtonXMLLoader& buttonLoader,
                       GHMessageHandler& messageQueue, GHIdentifier houseAdTransId);
    
    virtual void* create(const GHXMLNode& node, GHPropertyContainer& extraData) const;
    GH_NO_POPULATE
    
private:
    const GHGUIButtonXMLLoader& mButtonLoader;
    GHMessageHandler& mMessageQueue;
    GHIdentifier mTransitionId;
};
