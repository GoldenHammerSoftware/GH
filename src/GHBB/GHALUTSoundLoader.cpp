// Copyright 2010 Golden Hammer Software
#include "GHALUTSoundLoader.h"
#include <alut.h>
#include "GHOALErrorReporter.h"
#include "GHOALSoundHandle.h"
#include <cstdio>

GHALUTSoundLoader::GHALUTSoundLoader(GHOALSoundHandleMgr& soundHandleMgr, const GHString& path)
	: mSoundHandleMgr(soundHandleMgr)
	, mPath(path)
{
	
}

GHResource* GHALUTSoundLoader::loadFile(const char* filename, GHPropertyContainer* extraData)
{
	char filenameBuffer[1024];
	::snprintf(filenameBuffer, 1024, "%s%s", mPath.getChars(), filename);

	ALenum format;
	ALsizei size;
	ALfloat frequency;

	ALvoid* data = alutLoadMemoryFromFile(filenameBuffer, &format, &size, &frequency);
	GHOALErrorReporter::checkAndReportError("Failed to load sound from file");

	return new GHOALSoundHandle(mSoundHandleMgr, format, size, data, frequency);
}
