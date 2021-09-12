// Copyright 2010 Golden Hammer Software
#include "GHCocoaTouchpadMouseSeparatorXMLLoader.h"
#include "GHCocoaTouchpadMouseSeparator.h"

GHCocoaTouchpadMouseSeparatorXMLLoader::GHCocoaTouchpadMouseSeparatorXMLLoader(GHCocoaView* cocoaView)
: mCocoaView(cocoaView)
{
    
}

void* GHCocoaTouchpadMouseSeparatorXMLLoader::create(const GHXMLNode& node, GHPropertyContainer& extraData) const
{
    return new GHCocoaTouchpadMouseSeparator(mCocoaView);
}
