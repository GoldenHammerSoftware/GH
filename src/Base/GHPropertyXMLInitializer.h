// Copyright Golden Hammer Software
#pragma once

class GHAppShard;
class GHSystemServices;

// a class to initialize the loaders for different property types.
class GHPropertyXMLInitializer
{
public:
    void initAppShard(GHAppShard& appShard, GHSystemServices& systemServices);
};
