// Copyright 2010 Golden Hammer Software
#pragma once

#include "GHPlatform/GHOutputPipe.h"

class GHMetroOutputPipe : public GHOutputPipe
{
public:
    virtual void outputString(const char* str);
};
