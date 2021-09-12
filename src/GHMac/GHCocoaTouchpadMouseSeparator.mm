// Copyright 2010 Golden Hammer Software
#include "GHCocoaTouchpadMouseSeparator.h"

GHCocoaTouchpadMouseSeparator::GHCocoaTouchpadMouseSeparator(GHCocoaView* cocoaView)
: mCocoaView(cocoaView)
{
    
}

void GHCocoaTouchpadMouseSeparator::activate(void)
{
    [mCocoaView separateTouchpadMouse:TRUE];
}

void GHCocoaTouchpadMouseSeparator::deactivate(void)
{
    [mCocoaView separateTouchpadMouse:FALSE];
}
