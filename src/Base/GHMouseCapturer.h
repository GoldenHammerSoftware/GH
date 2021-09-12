// Copyright Golden Hammer Software
#pragma once

// Interface for preventing a mouse cursor from leaving the window.
class GHMouseCapturer
{
public:
    virtual ~GHMouseCapturer(void) { }
    
	// hide the mouse and lock it to our window.
    virtual void captureMouse(void) = 0;
	// show the mouse and return it to the system.
    virtual void releaseMouse(void) = 0;
    
	// value in window pct
    virtual void setMousePos(float x, float y) = 0;
};
