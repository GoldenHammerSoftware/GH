// Copyright 2010 Golden Hammer Software
#pragma once

#include "GHPlatform/GHRefCounted.h"

// convenient way to put a IRandomAccessStream^ into a property map
class GHMetroRandomAccessStream : public GHRefCounted
{
public:
	Windows::Storage::Streams::IRandomAccessStream^ mStream;
};
