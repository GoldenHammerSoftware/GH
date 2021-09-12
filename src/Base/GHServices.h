// Copyright Golden Hammer Software
#pragma once

class GHAppShard;

//A Generic interface for adding extra services to GHSystemServices.
// Initially intended to be used for game-and-platform-specific stuff.
// Other services that are explicit (eg, RenderServices, GameServices, etc)
// do not need to derive from this. This is only for passing to a non-explicit list.
class GHServices
{
public:
	virtual ~GHServices(void) { }

	virtual void initAppShard(GHAppShard& appShard) = 0;
};
