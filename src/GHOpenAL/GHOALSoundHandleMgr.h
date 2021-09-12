// Copyright 2010 Golden Hammer Software
#pragma once

#include "GHOALInclude.h"
#include <vector>

// A class to manage the OAL sound handles, similar to GHTextureMgrOGL but for sounds.
class GHOALSoundHandleMgr
{
public:
	GHOALSoundHandleMgr(unsigned int maxBuffers, unsigned int maxEmitters);
	~GHOALSoundHandleMgr(void);
	
	ALuint getUnusedBufferHandle(void);
	void returnBufferHandleToPool(ALuint id);

	ALuint getUnusedEmitterHandle(void);
	void returnEmitterHandleToPool(ALuint id);
	
protected:
	unsigned int mNumBuffers;
	ALuint* mBuffers;
	std::vector<ALuint> mUnusedBufferHandles;

	unsigned int mNumEmitters;
	ALuint* mEmitters;
	std::vector<ALuint> mUnusedEmitterHandles;
};
