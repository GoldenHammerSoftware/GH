// Copyright 2010 Golden Hammer Software
#import <UIKit/UIKit.h>
#import "GHiOSView.h"

@interface GHiOSViewController : UIViewController
{
@protected
    IBOutlet GHiOSView* mGHView;
}
@property (nonatomic, retain) GHiOSView* mGHView;
@end
