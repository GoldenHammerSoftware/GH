#import "GHTestAppAppDelegate.h"
#include "GHTestApp.h"

@implementation GHTestAppAppDelegate

@synthesize window;
@synthesize view;

- (void)applicationDidFinishLaunching:(NSNotification *)aNotification
{
    mView = view;
    mWindow = window;
    [super applicationDidFinishLaunching:aNotification];
}

- (GHApp*)createApp
{
    return new GHTestApp(*mSystemServices, *mRenderServices, *mGameServices);
}

@end
