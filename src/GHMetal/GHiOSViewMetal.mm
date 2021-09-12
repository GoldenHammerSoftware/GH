// Copyright Golden Hammer Software
#import "GHiOSViewMetal.h"

@implementation GHiOSViewMetal

// You must implement this
+ (Class)layerClass {
    return [CAMetalLayer class];
}

- (void)touchesBegan:(NSSet *)touches withEvent:(UIEvent *)event
{
    mBehavior.touchesBegan(touches, event, self);
}

- (void)touchesMoved:(NSSet *)touches withEvent:(UIEvent *)event
{
    mBehavior.touchesMoved(touches, event, self);
}

- (void)touchesEnded:(NSSet *)touches withEvent:(UIEvent *)event
{
    mBehavior.touchesEnded(touches, event);
}

- (void)touchesCancelled:(NSSet *)touches withEvent:(UIEvent *)event
{
    [self touchesEnded:touches withEvent:event];
}

- (void)layoutSubviews {
}

- (id)initWithCoder:(NSCoder*)coder
{
    NSString* outStr = [[NSString alloc] initWithUTF8String:"initWithCoder"];
    NSLog(@"%@", outStr);
    
    if ((self = [super initWithCoder:coder]))
    {
        // todo: window width.
    }
    return self;
}

-(void) handleViewResize
{
    CGSize viewSize = self.bounds.size;
    mBehavior.handleViewResize(viewSize);
}

- (void)insertText:(NSString *)text
{
    mBehavior.insertText(text);
}
- (void)deleteBackward
{
    mBehavior.deleteBackward();
}
- (BOOL)hasText {
    // Return whether there's any text present
    return YES;
}
- (BOOL)canBecomeFirstResponder {
    return YES;
}

- (void)controllerStateChanged
{
    mBehavior.controllerStateChanged();
}

- (BOOL)becomeFirstResponder
{
    // this function allows a keyboard to appear.
    // we were having trouble with admob popping up a keyboard
    //  when we didn't want one.
    if (mBehavior.getAllowKeyboard())
    {
        BOOL ret = [super becomeFirstResponder];
        return ret;
    }
    return NO;
}

@end
