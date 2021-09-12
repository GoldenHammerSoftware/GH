// Copyright 2010 Golden Hammer Software
#pragma once

#include "GHSoundHandle.h"

#include <SLES/OpenSLES.h>
#include <SLES/OpenSLES_Android.h>

// class to manage the lifetime of file descriptor asset construction arguments.
// they seem to go out of scope after GHOpenSLLoader finishes.
// some of these might not actually be needed in here.
class GHOpenSLHandleFDArgs
{
public:
	off_t start, length;
	int fd;
	SLDataLocator_AndroidFD loc_fd;
	SLDataFormat_MIME format_mime;
	SLDataSource audioSrc;
};

// interface for a sound that can be played through an emitter
class GHOpenSLHandle : public GHSoundHandle
{
public:
	GHOpenSLHandle(GHOpenSLHandleFDArgs* fdArgs);
	virtual ~GHOpenSLHandle(void);

	virtual GHByteStream* getByteStream(void);
	virtual size_t getDataSize(void) const;
	virtual unsigned short getNumChannels(void) const;
	virtual unsigned short getSamplesPerSecond(void) const;
	virtual unsigned short getBitsPerSample(void) const;

	SLDataSource& getDataSource(void);

private:
	GHOpenSLHandleFDArgs* mFDArgs;
};
