// Copyright 2010 Golden Hammer Software
#pragma once

#include "GHResourceLoader.h"

class GHFileOpener;

class GHWPSoundLoader : public GHResourceLoader
{
public:
	GHWPSoundLoader(GHFileOpener& fileOpener);

	virtual GHResource* loadFile(const char* filename, GHPropertyContainer* extraData);

private:
	GHFileOpener& mFileOpener;
};