// Copyright 2010 Golden Hammer Software
#pragma once

#include "GHUtils/GHResourceLoader.h"

class GHAndroidSoundPool;

class GHAndroidSoundLoader : public GHResourceLoader
{
public:
	GHAndroidSoundLoader(GHAndroidSoundPool& mediaPlayer);
    
	virtual GHResource* loadFile(const char* filename, GHPropertyContainer* extraData);
    
private:
    GHAndroidSoundPool& mMediaPlayer;
};
