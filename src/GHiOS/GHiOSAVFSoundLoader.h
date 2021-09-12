// Copyright 2010 Golden Hammer Software
#pragma once

#include "GHUtils/GHResourceLoader.h"
#include "GHiOSFilenameCreator.h"

class GHSoundHandle;

class GHiOSAVFSoundLoader : public GHResourceLoader
{
public:
	GHiOSAVFSoundLoader(void);
	
	virtual GHResource* loadFile(const char* filename, GHPropertyContainer* extraData);
	
private:
    void extractMetaData(GHSoundHandle& ret, const char* filename);

protected:
	GHiOSFilenameCreator mFileNameCreator;
};
