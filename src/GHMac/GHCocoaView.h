// Copyright 2010 Golden Hammer Software
#pragma once

class GHInputState;
class GHMutex;

@interface GHCocoaView : NSView 
{
@protected
    GHInputState* mInputState;
    GHMutex* mInputMutex;
    NSTouch* mTouches[10];
    bool mReportDeltas;
    bool mSeparateTouchpadFromMouse;
}
-(void) initVars;
-(void) setInputState:(GHInputState*)state;
-(void) setInputMutex:(GHMutex*)mutex;
-(void) clearInput;
-(void) setDeltaReporting:(bool)report;
-(void) sendTouchPosToApp:(int)index withX:(float) posX andY:(float) posY;
- (void) separateTouchpadMouse:(BOOL)separate;
@end
