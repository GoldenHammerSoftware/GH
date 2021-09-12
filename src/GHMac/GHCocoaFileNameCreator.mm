// Copyright 2010 Golden Hammer Software
#include "GHCocoaFileNameCreator.h"

GHCocoaFileNameCreator::GHCocoaFileNameCreator(void)
{
	mFileManager = [NSFileManager defaultManager];
	NSString* bundlePath = [[NSBundle mainBundle] bundlePath];
	mDocumentsDirectory = [bundlePath stringByAppendingPathComponent:@"Contents/Resources"];
}

GHCocoaFileNameCreator::~GHCocoaFileNameCreator(void)
{
}

bool GHCocoaFileNameCreator::createFileName(const char* filename, __strong NSString*& ret)
{
	NSString* nsFilename = [[NSString alloc] initWithUTF8String:filename];
    ret = [mDocumentsDirectory stringByAppendingPathComponent:nsFilename];
	if (![mFileManager fileExistsAtPath:ret]) {
		//GHDebugMessage::outputDebugString("Unable to find %s on disk.\n", filename);
		return false;
	}
	return true;
}

void GHCocoaFileNameCreator::createWriteableFileName(const char* filename, __strong NSString*& ret)
{
    NSString* nsFilename = [[NSString alloc] initWithUTF8String:filename];
    ret = [mDocumentsDirectory stringByAppendingPathComponent:nsFilename];
}
