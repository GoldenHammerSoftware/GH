// Copyright 2010 Golden Hammer Software
#include "GHiOSSoundTransition.h"
#include <AudioToolbox/AudioServices.h>
#include "GHPlatform/GHDebugMessage.h"
#import <UIKit/UIKit.h>

void GHiOSSoundTransition::activate(void)
{
    NSString *version = [[UIDevice currentDevice] systemVersion];
    BOOL isAtLeastiOS8 = [version floatValue] >= 8.0;
    if (!isAtLeastiOS8)
    {
        // AudioSessionSetActive is deprecated in ios7 and seems to have stopped working in 8
        AudioSessionSetActive(YES);
    }
}

void GHiOSSoundTransition::deactivate(void)
{
    NSString *version = [[UIDevice currentDevice] systemVersion];
    BOOL isAtLeastiOS8 = [version floatValue] >= 8.0;
    if (!isAtLeastiOS8)
    {
        AudioSessionSetActive(NO);
    }
}
