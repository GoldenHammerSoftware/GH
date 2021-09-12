// Copyright 2010 Golden Hammer Software
#include "GHAndroidSoundLoader.h"
#include "GHAndroidSoundPool.h"
#include "GHAndroidSoundPoolHandle.h"

GHAndroidSoundLoader::GHAndroidSoundLoader(GHAndroidSoundPool& mediaPlayer)
: mMediaPlayer(mediaPlayer)
{
    
}

GHResource* GHAndroidSoundLoader::loadFile(const char* filename, GHPropertyContainer* extraData)
{
	return new GHAndroidSoundPoolHandle(filename);
}
