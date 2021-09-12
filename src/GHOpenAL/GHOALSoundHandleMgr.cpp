// Copyright 2010 Golden Hammer Software
#include "GHOALSoundHandleMgr.h"
#include "GHOALErrorReporter.h"
#include "GHPlatform/GHDebugMessage.h"

GHOALSoundHandleMgr::GHOALSoundHandleMgr(unsigned int maxBuffers, unsigned int maxEmitters)
:	mNumBuffers(maxBuffers)
,	mNumEmitters(maxEmitters)
{
	mBuffers = new ALuint[mNumBuffers];
	alGenBuffers(mNumBuffers, mBuffers);
	GHOALErrorReporter::checkAndReportError("alGenBuffers");
	for (int i = 0; i < mNumBuffers; ++i) {
		mUnusedBufferHandles.push_back(mBuffers[i]);
	}

	mEmitters = new ALuint[mNumEmitters];
	alGenSources(mNumEmitters, mEmitters);
	GHOALErrorReporter::checkAndReportError("alGenSources");
	for (int i = 0; i < mNumEmitters; ++i) {
		mUnusedEmitterHandles.push_back(mEmitters[i]);
	}
}

GHOALSoundHandleMgr::~GHOALSoundHandleMgr(void)
{
	alDeleteBuffers(mNumBuffers, mBuffers);
	alDeleteSources(mNumEmitters, mEmitters);
	delete [] mBuffers;
	delete [] mEmitters;
}

ALuint GHOALSoundHandleMgr::getUnusedBufferHandle(void)
{
	if (!mUnusedBufferHandles.size()) {
		GHDebugMessage::outputString("Asking for a sound buffer handle when none are available!\n");
		return 0;
	}
	ALuint ret = mUnusedBufferHandles[mUnusedBufferHandles.size()-1];
	mUnusedBufferHandles.pop_back();
	return ret;
}

void GHOALSoundHandleMgr::returnBufferHandleToPool(ALuint id)
{
	mUnusedBufferHandles.push_back(id);
}

ALuint GHOALSoundHandleMgr::getUnusedEmitterHandle(void)
{
	if (!mUnusedEmitterHandles.size()) {
		GHDebugMessage::outputString("Asking for a sound emitter handle when none are available!\n");
		return 0;
	}
	ALuint ret = mUnusedEmitterHandles[mUnusedEmitterHandles.size()-1];
	mUnusedEmitterHandles.pop_back();
	return ret;
}

void GHOALSoundHandleMgr::returnEmitterHandleToPool(ALuint id)
{
	mUnusedEmitterHandles.push_back(id);
}


