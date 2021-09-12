// Copyright 2010 Golden Hammer Software
#pragma once

#include "GHOutputPipe.h"

class GHCocoaOutputPipe : public GHOutputPipe
{
public:
	virtual void outputString(const char* str);
};
