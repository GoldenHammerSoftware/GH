// Copyright 2010 Golden Hammer Software
#pragma once

#include "GHUtils/GHResourceLoader.h"

class GHOALSoundHandleMgr;
class GHAppleFilenameCreator;

class GHAppleOALSoundLoader : public GHResourceLoader
{
public:
	GHAppleOALSoundLoader(GHOALSoundHandleMgr& handleMgr,
                          GHAppleFilenameCreator* filenameCreator);
    ~GHAppleOALSoundLoader(void);
	
	virtual GHResource* loadFile(const char* filename, GHPropertyContainer* extraData);
	
protected:
	GHOALSoundHandleMgr& mHandleMgr;
	GHAppleFilenameCreator* mFileNameCreator;
};
