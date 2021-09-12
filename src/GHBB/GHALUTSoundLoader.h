// Copyright 2010 Golden Hammer Software
#pragma once

#include "GHResourceLoader.h"
#include "GHString.h"

class GHOALSoundHandleMgr;

class GHALUTSoundLoader : public GHResourceLoader
{
public:
	GHALUTSoundLoader(GHOALSoundHandleMgr& soundHandleMgr, const GHString& path);

	virtual GHResource* loadFile(const char* filename, GHPropertyContainer* extraData);

private:
	GHOALSoundHandleMgr& mSoundHandleMgr;
	GHString mPath;
};
