// Copyright Golden Hammer Software
#pragma once

#include "Render/GHRenderServices.h"

class GHNullRenderServices : public GHRenderServices
{
public:
    GHNullRenderServices(GHSystemServices& systemServices);
};
