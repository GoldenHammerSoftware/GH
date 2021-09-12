#import "GHCocoaAppDelegate.h"
#import "GHCocoaView.h"

@interface GHRigidBodyToolAppDelegate : GHCocoaAppDelegate
{
@private
    NSWindow* window;
    GHCocoaView* view;
}

@property (assign) IBOutlet NSWindow *window;
@property (assign) IBOutlet GHCocoaView* view;

@end
