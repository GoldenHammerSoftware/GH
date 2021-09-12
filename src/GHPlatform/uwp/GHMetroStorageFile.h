// Copyright 2010 Golden Hammer Software
#pragma once

#include "GHPlatform/GHRefCounted.h"

// a way to stick a Windows::Storage::StorageFile^ into a property
class GHMetroStorageFile: public GHRefCounted
{
public:
	Windows::Storage::StorageFile^ mStorageFile;
};
