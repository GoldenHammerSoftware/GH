// Copyright Golden Hammer Software
#pragma once

#include "GHPlatform/GHOutputPipe.h"

class GHWin32OutputPipe : public GHOutputPipe
{
public:
	virtual void outputString(const char* str);
};
