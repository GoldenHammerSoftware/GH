// Copyright Golden Hammer Software
#pragma once

#include "GHXMLObjLoader.h"

class GHMessageXMLLoader;
class GHMessageHandler;

/*
 <messageSender>
    <message id=CHANGEMAINSTATE>
        <prop name=state><enumProp val=BS_INPUT/></prop>
    </message>
 </messageSender>
*/
class GHMessageSenderTransitionXMLLoader : public GHXMLObjLoader
{
public:
    GHMessageSenderTransitionXMLLoader(const GHMessageXMLLoader& messageLoader,
                                       GHMessageHandler& messageHandler);
    
    virtual void* create(const GHXMLNode& node, GHPropertyContainer& extraData) const;
    virtual void populate(void* obj, const GHXMLNode& node, GHPropertyContainer& extraData) const;
    
private:
    const GHMessageXMLLoader& mMessageLoader;
    GHMessageHandler& mMessageHandler;
};
