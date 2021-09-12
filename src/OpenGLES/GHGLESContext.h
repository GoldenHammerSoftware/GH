// Copyright 2010 Golden Hammer Software
#pragma once
// no ogl include.

// interface for a platform OGL context
// such as CAEAGL on iphone.
class GHGLESContext
{
public:
	virtual ~GHGLESContext(void) {}

	virtual void beginFrame(void) = 0;
	virtual void endFrame(void) = 0;

	virtual void setReinitNeeded(void) {}
	virtual void handleGraphicsQualityChanged(void) {}
};
