// Copyright 2010 Golden Hammer Software
#pragma once

#include "GHXMLObjLoader.h"
#import "GHCocoaView.h"

class GHCocoaTouchpadMouseSeparatorXMLLoader : public GHXMLObjLoader
{
public:
    GHCocoaTouchpadMouseSeparatorXMLLoader(GHCocoaView* cocoaView);
    
    virtual void* create(const GHXMLNode& node, GHPropertyContainer& extraData) const;
    GH_NO_POPULATE;
    
private:
    GHCocoaView* mCocoaView;
};
