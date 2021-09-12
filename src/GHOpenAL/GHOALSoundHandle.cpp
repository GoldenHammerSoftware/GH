// Copyright 2010 Golden Hammer Software
#include "GHOALSoundHandle.h"
#include "GHOALSoundHandleMgr.h"
#include "GHOALErrorReporter.h"

GHOALSoundHandle::GHOALSoundHandle(GHOALSoundHandleMgr& handleMgr, ALenum format, ALsizei size,
									 ALvoid* data, ALsizei freq)
:	mHandleMgr(handleMgr)
,	mBufferId(mHandleMgr.getUnusedBufferHandle())
{
	alBufferData(mBufferId, format, data, size, freq);
	GHOALErrorReporter::checkAndReportError("alBufferData");
}

GHOALSoundHandle::~GHOALSoundHandle(void)
{
	mHandleMgr.returnBufferHandleToPool(mBufferId);
}

