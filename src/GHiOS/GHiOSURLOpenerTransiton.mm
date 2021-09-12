// Copyright 2010 Golden Hammer Software
#include "GHiOSURLOpenerTransiton.h"
#import <UIKit/UIKit.h>

GHiOSURLOpenerTransiton::GHiOSURLOpenerTransiton(const char* url)
: mURLString(url, GHString::CHT_COPY)
{
}

void GHiOSURLOpenerTransiton::activate(void)
{
    NSString* nsurl;
    // special case for the device settings
    // this is cheating a little bit.
    if (!strcmp(mURLString.getChars(), "devicesettings"))
    {
        nsurl = UIApplicationOpenSettingsURLString;
    }
    else
    {
        nsurl = [NSString stringWithCString:mURLString.getChars() encoding:[NSString defaultCStringEncoding]];
    }

    dispatch_async(dispatch_get_main_queue(), ^{
        [[UIApplication sharedApplication] openURL:[NSURL URLWithString:nsurl]];
    });
}
