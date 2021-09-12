// Copyright Golden Hammer Software
#pragma once

// interface for manipulating the window where available
class GHWindow
{
public:
	virtual ~GHWindow(void) {}

	virtual void resize(int width, int height) = 0;
	virtual void setFullscreen(bool isFullscreen) = 0;
};
