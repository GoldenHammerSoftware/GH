// Copyright 2010 Golden Hammer Software
#pragma once

#include "GHSystemServices.h"
#include "GHMetroSoundFinder.h"

class GHControllerMgr;

class GHMetroSystemServices : public GHSystemServices
{
public:
	GHMetroSystemServices(GHControllerMgr& controllerManager);

	virtual void initAppShard(GHAppShard& appShard);

public:
	static const int sMaxPointers = 10;
	GHMetroSoundFinder mSoundFinder;
};