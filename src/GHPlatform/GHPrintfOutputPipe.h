// Copyright Golden Hammer Software
#pragma once

#include "GHOutputPipe.h"

class GHPrintfOutputPipe : public GHOutputPipe
{
public:
	virtual void outputString(const char* str);
};
