// Copyright 2010 Golden Hammer Software
#import "GHCocoaView.h"
#include "GHInputState.h"
#include "GHKeyDef.h"
#include "GHMutex.h"

@implementation GHCocoaView

static int sMaxTouches = 10;

- (void)initVars
{
	for (int i = 0; i < sMaxTouches; ++i) {
		mTouches[i] = 0;
	}
	mReportDeltas = false;
    mSeparateTouchpadFromMouse = false;
}

-(void)setInputState:(GHInputState*)state
{
    mInputState = state;
}

-(void) setInputMutex:(GHMutex*)mutex
{
    mInputMutex = mutex;
}

-(void) clearInput
{
	for (int i = 1; i < sMaxTouches; ++i) 
	{
		if (mTouches[i])
		{
			mTouches[i] = 0;
		}
	}	
}

- (void)setDeltaReporting:(bool)report
{
	mReportDeltas = report;
}

- (void)mouseMoved:(NSEvent *)theEvent
{
    if (mSeparateTouchpadFromMouse)
    {
        if ([theEvent subtype] == NSTouchEventSubtype)
        {
            return;
        }
    }
    
	float posX, posY;
	if (!mInputState || !mInputMutex) return;
    
    mInputMutex->acquire();
    
    // set the pointer active on the first move.
    if (!mInputState->getPointerActive(0))
    {
        mInputState->handlePointerActive(0, true);   
    }
    
    if (mReportDeltas) {
        posX = [theEvent deltaX];
        posY = [theEvent deltaY];
        float pctX = posX / [self bounds].size.width;
        float pctY = posY / [self bounds].size.height;
        mInputState->handlePointerDelta(0, GHPoint2(pctX, pctY));
    }
    else {
        posX = [theEvent locationInWindow].x;
        posY = [theEvent locationInWindow].y;
        float pctX = posX / [self bounds].size.width;
        float pctY = 1.0 - (posY / [self bounds].size.height);
        mInputState->handlePointerChange(0, GHPoint2(pctX, pctY));
    }
    mInputMutex->release();
}

- (void)mouseDragged:(NSEvent *)theEvent
{
	[self mouseMoved:theEvent];
}

- (void)rightMouseDragged:(NSEvent *)theEvent
{
	[self mouseMoved:theEvent];
}

- (void)mouseEntered:(NSEvent *)theEvent
{
}

- (void)mouseExited:(NSEvent *)theEvent
{
}

- (void)mouseDown:(NSEvent *)theEvent 
{
    if (!mInputState || !mInputMutex) return;
    mInputMutex->acquire();
	mInputState->handleKeyChange(0, GHKeyDef::KEY_MOUSE1, 1);
    mInputMutex->release();
}

- (void)mouseUp:(NSEvent *)theEvent 
{
    if (!mInputState || !mInputMutex) return;
    mInputMutex->acquire();
	mInputState->handleKeyChange(0, GHKeyDef::KEY_MOUSE1, 0);
    mInputMutex->release();
}

- (void)rightMouseDown:(NSEvent *)theEvent 
{
    if (!mInputState || !mInputMutex) return;
    mInputMutex->acquire();
	mInputState->handleKeyChange(0, GHKeyDef::KEY_MOUSE2, 1);
    mInputMutex->release();
}

- (void)rightMouseUp:(NSEvent *)theEvent 
{
    if (!mInputState || !mInputMutex) return;
    mInputMutex->acquire();
	mInputState->handleKeyChange(0, GHKeyDef::KEY_MOUSE2, 0);
    mInputMutex->release();
}

- (void)keyDown:(NSEvent *)theEvent 
{ 
    if (!mInputState || !mInputMutex) return;
	NSString *characters = [theEvent characters];
	if ([characters length]) {
		unsigned char keyPressed = [characters characterAtIndex:0];
        keyPressed = ::tolower(keyPressed);
        mInputMutex->acquire();
		mInputState->handleKeyChange(0, keyPressed, 1);
        mInputMutex->release();
	}
}

- (void)keyUp:(NSEvent *)theEvent 
{
    if (!mInputState || !mInputMutex) return;
	NSString *characters = [theEvent characters];
	if ([characters length]) {
		unsigned char keyPressed = [characters characterAtIndex:0];
        keyPressed = ::tolower(keyPressed);
        mInputMutex->acquire();
		mInputState->handleKeyChange(0, keyPressed, 0);
        mInputMutex->release();
	}
}

- (void)flagsChanged:(NSEvent *)event
{
    if (!mInputState || !mInputMutex) return;
    mInputMutex->acquire();
    unsigned flags = [event modifierFlags];
    if (flags & NSShiftKeyMask) 
    {
        mInputState->handleKeyChange(0, GHKeyDef::KEY_SHIFT, 1);
    }
    else
    {
        mInputState->handleKeyChange(0, GHKeyDef::KEY_SHIFT, 0);
    }
    
    if (flags & NSCommandKeyMask)
    {
        mInputState->handleKeyChange(0, GHKeyDef::KEY_CONTROL, 1);
    }
    else
    {
        mInputState->handleKeyChange(0, GHKeyDef::KEY_CONTROL, 0);
    }
    
    mInputMutex->release(); 
}

- (BOOL)wantsRestingTouches
{
    return YES;
}

int findTouchIndex(NSTouch* touch, __strong NSTouch** mTouches)
{
	// first look for an existing match.
	for (int i = 1; i < sMaxTouches; ++i) {
		if (mTouches[i] && touch)
		{
			NSTouch* currTouch = mTouches[i];
			if ([currTouch.identity isEqual:touch.identity])
			{
				return i;
			}
		}
	}
	// then look for an open slot.
	for (int i = 1; i < sMaxTouches; ++i) {
		if (mTouches[i] == 0) {
			mTouches[i] = touch;
			return i;
		}
	}	
	return -1;
}

- (void) sendTouchPosToApp:(int)index withX:(float) posX andY:(float) posY
{
    if (index == 1 && !mSeparateTouchpadFromMouse)
    {
        return;
    }
    
    if (!mInputState || !mInputMutex) return;

    mInputMutex->acquire();
	mInputState->handlePointerChange(index, GHPoint2(posX, posY));
    mInputMutex->release();
}

- (void)touchesBeganWithEvent:(NSEvent *)event
{
    if (!mInputState || !mInputMutex) return;
    
	NSSet* touchSet = [event touchesMatchingPhase:NSTouchPhaseBegan inView:nil];
	NSArray *touchArr = [touchSet allObjects];
    NSInteger touchCnt = [touchArr count];
	for (int i = 0; i < touchCnt; ++i) 
	{
		NSTouch *touch = [touchArr objectAtIndex:i];
		int index = findTouchIndex(touch, mTouches);
        
		if (index == -1) {
			continue;
		}
		NSPoint touchPoint = [touch normalizedPosition];

        mInputMutex->acquire();
		mInputState->handlePointerActive(index, true);
        mInputState->handleKeyChange(0, (int)GHKeyDef::KEY_MOUSE1 + (index*3), true);
        mInputMutex->release();
        
        [self sendTouchPosToApp:index withX:touchPoint.x andY:(1.0 - touchPoint.y)];
	}
}

- (void)touchesMovedWithEvent:(NSEvent *)event
{
    if (!mInputState || !mInputMutex) return;

	NSSet* touchSet = [event touchesMatchingPhase:NSTouchPhaseTouching inView:nil];
	NSArray *touchArr = [touchSet allObjects];
    NSInteger touchCnt = [touchArr count];
	for (int i = 0; i < touchCnt; ++i) 
	{
		NSTouch *touch = [touchArr objectAtIndex:i];
		int index = findTouchIndex(touch, mTouches);
		if (index == -1) {
			continue;
		}
		NSPoint touchPoint = [touch normalizedPosition];
        [self sendTouchPosToApp:index withX:touchPoint.x andY:(1.0 - touchPoint.y)];
	}
}

- (void)touchesEndedWithEvent:(NSEvent *)event
{
    if (!mInputState || !mInputMutex) return;

	NSSet* touchSet = [event touchesMatchingPhase:NSTouchPhaseEnded inView:nil];
	NSArray *touchArr = [touchSet allObjects];
    NSInteger touchCnt = [touchArr count];
	for (int i = 0; i < touchCnt; ++i) 
	{
		NSTouch *touch = [touchArr objectAtIndex:i];
		int index = findTouchIndex(touch, mTouches);
		
		if (index == -1) {
			continue;
		}
        mInputMutex->acquire();
        mInputState->handleKeyChange(0, (int)GHKeyDef::KEY_MOUSE1 + (index*3), false);
        mInputState->handlePointerActive(index, false);
        mInputMutex->release();

        [self sendTouchPosToApp:index withX:-1 andY:-1];
        mTouches[index] = 0;
	}		
}

- (void)touchesCancelledWithEvent:(NSEvent *)event
{
    [self touchesEndedWithEvent:event];
}

- (BOOL)acceptsFirstResponder {
    return YES;
}

- (void) separateTouchpadMouse:(BOOL)separate
{
    mSeparateTouchpadFromMouse = separate;
}

@end
