#ifndef GHMETAL

#import "GHiOSViewOgles.h"

@implementation GHiOSViewOgles

// You must implement this
+ (Class)layerClass {
    return [CAEAGLLayer class];
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

//The GL view is stored in the nib file. When it's unarchived it's sent -initWithCoder:
- (id)initWithCoder:(NSCoder*)coder
{
    NSString* outStr = [[NSString alloc] initWithUTF8String:"initWithCoder"];
    NSLog(@"%@", outStr);
    
    if ((self = [super initWithCoder:coder])) {
        // Get the layer
        CAEAGLLayer *eaglLayer = (CAEAGLLayer *)self.layer;
        eaglLayer.opaque = YES;
        eaglLayer.drawableProperties = [NSDictionary dictionaryWithObjectsAndKeys:
                                        [NSNumber numberWithBool:NO], kEAGLDrawablePropertyRetainedBacking,
                                        /*kEAGLColorFormatRGB565,*/
                                        kEAGLColorFormatRGBA8,
                                        kEAGLDrawablePropertyColorFormat,
                                        nil];
        
        CGRect screenBounds = eaglLayer.bounds;
        int widthToUse = screenBounds.size.width;
        int heightToUse = screenBounds.size.height;
        
        /* // contentscalefactor can be pretty slow on some devices.  trading off quality for speed here for now
        // set up the scale factor for our rendering based on the main window scale.
        UIScreen* mainscr = [UIScreen mainScreen];
        if (mainscr)
        {
            CGFloat scale = [mainscr scale];
            // calling this will tell egl to use more pixels if >1.
            // limit to 1.5 for perf until we can tie this to graphics quality.
            if (scale > 1.5) scale = 1.5;
            self.contentScaleFactor = scale;
        }
        */
        
        mBehavior.setWindowWidth(widthToUse);
        mBehavior.setWindowHeight(heightToUse);
    }
    return self;
}

-(void) handleViewResize
{
    CGSize viewSize = self.bounds.size;
    mBehavior.handleViewResize(viewSize);
}

- (CAEAGLLayer*)getEAGLLayer
{
    CAEAGLLayer* eaglLayer = (CAEAGLLayer*)self.layer;
    return eaglLayer;
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

#endif // ghmetal
