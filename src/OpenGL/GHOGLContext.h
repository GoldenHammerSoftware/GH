// Copyright 2010 Golden Hammer Software
#pragma once

// something to hide the system calls from the opengl renderer (agl, wgl, glut, cocoa)
class GHOGLContext
{
public:
	virtual ~GHOGLContext(void) {}
	
	virtual void beginFrame(void) = 0;
	virtual void swapBuffers(void) = 0;
    virtual void setFullScreen(bool isFull) = 0;
};
