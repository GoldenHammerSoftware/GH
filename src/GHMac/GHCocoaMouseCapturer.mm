// Copyright 2010 Golden Hammer Software
#include "GHCocoaMouseCapturer.h"
#include "GHDebugMessage.h"

GHCocoaMouseCapturer::GHCocoaMouseCapturer(GHCocoaView* view)
: mView(view)
, mMouseVisCount(1)
{
    
}

void GHCocoaMouseCapturer::captureMouse(void)
{
    --mMouseVisCount;
    handleMouseVisCountChange();
}

void GHCocoaMouseCapturer::releaseMouse(void)
{
    ++mMouseVisCount;
    handleMouseVisCountChange();
}

void GHCocoaMouseCapturer::handleMouseVisCountChange(void)
{
    CGAssociateMouseAndMouseCursorPosition(mMouseVisCount>0);
	[mView setDeltaReporting:(mMouseVisCount<1)];
	
	CGError error;
	if (mMouseVisCount > 0) 
	{
		error = CGDisplayShowCursor(kCGDirectMainDisplay);
	}
	else 
	{
		error = CGDisplayHideCursor(kCGDirectMainDisplay);
	}
	
	if (error != kCGErrorSuccess) {
		GHDebugMessage::outputString("Error changing cursor visibility: %d", error );
	}
}

void GHCocoaMouseCapturer::setMousePos(float x, float y)
{
    CGPoint point;
	point.x = (int)x;
	point.y = (int)y;
	CGWarpMouseCursorPosition(point);
}
