// Copyright Golden Hammer Software
#pragma once

#include "GHRenderServices.h"

class GHNullRenderServices : public GHRenderServices
{
public:
    GHNullRenderServices(GHSystemServices& systemServices);
};
