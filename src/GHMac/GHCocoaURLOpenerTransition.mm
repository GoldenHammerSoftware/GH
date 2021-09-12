// Copyright 2010 Golden Hammer Software
#include "GHCocoaURLOpenerTransition.h"

GHCocoaURLOpenerTransition::GHCocoaURLOpenerTransition(const char* url)
: mURLString(url, GHString::CHT_COPY)
{
}

void GHCocoaURLOpenerTransition::activate(void)
{
    NSWorkspace* ws = [NSWorkspace sharedWorkspace];
    NSString* nsurlstr = [NSString stringWithCString:mURLString.getChars() encoding:[NSString defaultCStringEncoding]];
    NSURL* url = [NSURL URLWithString:nsurlstr];
    
    [ws openURL:url];
}
