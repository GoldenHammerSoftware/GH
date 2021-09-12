// Copyright 2010 Golden Hammer Software
#include "GHAndroidSoundHandle.h"
#include "GHAndroidSoundPool.h"

GHAndroidSoundHandle::GHAndroidSoundHandle(GHAndroidSoundPool& mediaPlayer, jobject mediaPlayerObj)
: mMediaPlayer(mediaPlayer)
, mMediaPlayerObj(mediaPlayerObj)
{
}

GHAndroidSoundHandle::~GHAndroidSoundHandle(void)
{
    mMediaPlayer.releaseSound(mMediaPlayerObj);
}

