// Copyright 2010 Golden Hammer Software
#include "GHiOSFilenameCreator.h"

bool GHiOSFilenameCreator::createFileName(const char* filename, __strong NSString*& ret)
{
    NSArray *paths = NSSearchPathForDirectoriesInDomains(NSDocumentDirectory, NSUserDomainMask, YES);
    NSString *documentsDirectory = [paths objectAtIndex:0];
    NSString* nsDocumentsFilename = [[NSString alloc] initWithUTF8String:filename];
    NSString* bundlePath = [documentsDirectory stringByAppendingPathComponent:nsDocumentsFilename];
    if ([[NSFileManager defaultManager] fileExistsAtPath:bundlePath])
    {
        // found inside the documents directory
        ret = bundlePath;
        return true;
    }
    
	NSString* nsFilename = [[NSString alloc] initWithUTF8String:filename];
    ret = [[[NSBundle mainBundle] bundlePath] stringByAppendingPathComponent:nsFilename];
	if ([[NSFileManager defaultManager] fileExistsAtPath:ret])
    {
        // found inside the bundle
		return true;
	}
	return false;
}

void GHiOSFilenameCreator::createWriteableFileName(const char* filename, __strong NSString*& ret)
{
    NSArray *paths = NSSearchPathForDirectoriesInDomains(NSDocumentDirectory, NSUserDomainMask, YES);
    NSString *documentsDirectory = [paths objectAtIndex:0];
    NSString* nsDocumentsFilename = [[NSString alloc] initWithUTF8String:filename];
    ret = [documentsDirectory stringByAppendingPathComponent:nsDocumentsFilename];
}
