#pragma once
#ifndef GHMETAL

#include "GHiOSViewBehavior.h"

#import <UIKit/UIKit.h>
#import <QuartzCore/QuartzCore.h>

@interface GHiOSViewOgles : UIView <UIKeyInput>
{
    @public GHiOSViewBehavior mBehavior;
}

-(void) controllerStateChanged;
-(void) handleViewResize;

- (CAEAGLLayer*)getEAGLLayer;

@end

#endif
