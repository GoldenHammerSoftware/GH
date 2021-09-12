#import "CGAppDelegate.h"
#include "CGApp.h"

@implementation CGAppDelegate

@synthesize window;
@synthesize view;

- (void)applicationDidFinishLaunching:(NSNotification *)aNotification
{
    mView = view;
    mWindow = window;
    [window setContentAspectRatio:NSMakeSize(1.6,1.0)];
    [super applicationDidFinishLaunching:aNotification];
}

- (GHApp*)createApp
{
    return new CGApp(*mSystemServices, *mRenderServices, *mGameServices, *mAppMessageQueue);
}

@end
