// Copyright 2010 Golden Hammer Software
#pragma once

#include "GHSystemServices.h"

class GHControllerMgr;

class GHWPSystemServices : public GHSystemServices
{
public:
	GHWPSystemServices(GHControllerMgr& systemControllerMgr);

	virtual void initAppShard(GHAppShard& appShard);

	bool checkExitOnBack(void) const { return mExitOnBack; }

public:
	static const int sMaxPointers = 10;
	// a flag to say whether we should exit when the back button is pressed.
	bool mExitOnBack;
};
