// Copyright Golden Hammer Software
#pragma once

#include "Base/GHMouseCapturer.h"

class GHNullMouseCapturer : public GHMouseCapturer
{
public:
    virtual void captureMouse(void) {}
    virtual void releaseMouse(void) {}
	virtual void setMousePos(float x, float y) {}
};
