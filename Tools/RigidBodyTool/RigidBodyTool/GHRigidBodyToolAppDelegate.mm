#import "GHRigidBodyToolAppDelegate.h"
#include "GHRigidBodyTool.h"

@implementation GHRigidBodyToolAppDelegate

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
    return new GHRigidBodyTool(*mSystemServices, *mRenderServices, *mGameServices);
}

@end