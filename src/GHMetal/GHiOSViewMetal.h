// Copyright Golden Hammer Software
#pragma once

#include "GHiOSViewBehavior.h"

#import <UIKit/UIKit.h>
#import <QuartzCore/QuartzCore.h>

#import <MetalKit/MetalKit.h>

@interface GHiOSViewMetal : MTKView <UIKeyInput>
{
    @public GHiOSViewBehavior mBehavior;
}

-(void) controllerStateChanged;
-(void) handleViewResize;

@end
