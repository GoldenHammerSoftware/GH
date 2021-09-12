// Copyright Golden Hammer Software
#pragma once

class GHAppShard;
class GHSystemServices;
class GHPropertyContainerXMLLoader;

// wraps initializing all the xml loaders.
namespace GHBaseXMLInitializer
{
	void initXMLLoaders(GHAppShard& appShard, 
		GHSystemServices& systemServices);
};
