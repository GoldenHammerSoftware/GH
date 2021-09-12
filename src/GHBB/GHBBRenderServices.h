// Copyright 2010 Golden Hammer Software
#pragma once

#include "GHGLESRenderServices.h"
#include "GHNullMutex.h"
#include <screen/screen.h>

class GHBBGLESContext;

class GHBBRenderServices : public GHGLESRenderServices
{
public:
	GHBBRenderServices(GHSystemServices& systemServices, const GHStringHashTable& hashTable,
			screen_context_t& ctx);
	~GHBBRenderServices(void);

	virtual void initAppShard(GHAppShard& appShard);

	GHBBGLESContext& getContext(void) { return *mContext; }

private:
	GHNullMutex mMutex;
	GHBBGLESContext* mContext;
};
