// Copyright Golden Hammer Software
#pragma once

#include "GHStringIdFactory.h"

// Simple implementation of the DJB2 hashing algorithm
// Currently does not handle collisions
class GHStringIdFactoryDJB2 : public GHStringIdFactory
{
public:
	virtual GHIdentifier generateHash(const char* str, size_t len=0) const;
};
