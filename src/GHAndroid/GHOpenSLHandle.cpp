// Copyright 2010 Golden Hammer Software
#include "GHOpenSLHandle.h"
#include <SLES/OpenSLES.h>
#include <SLES/OpenSLES_Android.h>

GHOpenSLHandle::GHOpenSLHandle(GHOpenSLHandleFDArgs* fdArgs)
: mFDArgs(fdArgs)
{
}

GHOpenSLHandle::~GHOpenSLHandle(void)
{
	if (mFDArgs) delete mFDArgs;
}

GHByteStream* GHOpenSLHandle::getByteStream(void)
{
	// todo
	return 0;
}

size_t GHOpenSLHandle::getDataSize(void) const
{
	// todo
	return 0;
}

unsigned short GHOpenSLHandle::getNumChannels(void) const
{
	// todo
	return 0;
}

unsigned short GHOpenSLHandle::getSamplesPerSecond(void) const
{
	// todo
	return 0;
}

unsigned short GHOpenSLHandle::getBitsPerSample(void) const
{
	// todo
	return 0;
}

SLDataSource& GHOpenSLHandle::getDataSource(void) 
{ 
	return mFDArgs->audioSrc;
}
