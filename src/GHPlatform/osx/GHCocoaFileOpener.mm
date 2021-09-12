// Copyright 2010 Golden Hammer Software
#include "GHCocoaFileOpener.h"
#include "GHFileC.h"
#import <Foundation/Foundation.h>

GHCocoaFileOpener::GHCocoaFileOpener(void)
{
    // we need to create the save directory in order to put any files in there.
    NSFileManager *fileManager = [NSFileManager defaultManager];
    NSString* ourDocumentsDirectory = getDocumentsPath();
    BOOL isDirectory = NO;
    BOOL exists = [fileManager fileExistsAtPath:ourDocumentsDirectory isDirectory:&isDirectory];
    if (!exists)
    {
        NSError *error;
        [fileManager createDirectoryAtPath:ourDocumentsDirectory withIntermediateDirectories:YES attributes:nil error:&error];
    }
}

GHFile* GHCocoaFileOpener::openPlatformFile(const char* filename,
                                            GHFile::FileType fileType, 
                                            GHFile::FileMode fileMode) const
{
	if (!filename) return 0;
    
    const char* flags = "rt";
    if (fileMode == GHFile::FM_READONLY)
    {
        if (fileType == GHFile::FT_BINARY) flags = "rb";
    }
    else
    {
        if (fileType == GHFile::FT_BINARY) flags = "wb";
        else flags = "wt";
    }
    
    
	// check documents directory first, failing that check the bundle.
	FILE* ret = openDocumentsFile(filename, flags);
	if (!ret) {
		ret = openBundleFile(filename, flags);
	}
    if (!ret) return 0;
	return new GHFileC(ret, fileType, fileMode);
}

FILE* GHCocoaFileOpener::openBundleFile(const char* filename, const char* flags) const
{
	NSFileManager *fileManager = [NSFileManager defaultManager];
	NSString* documentsDirectory = [[NSBundle mainBundle] bundlePath];
	
	NSString* nsFilename = 0;
	nsFilename = [[NSString alloc] initWithUTF8String:filename];
	NSString* subpath = [documentsDirectory stringByAppendingPathComponent:@"Contents/Resources"];
    NSString* path = [subpath stringByAppendingPathComponent:nsFilename];
	
	// can't open write files in the bundle directory, so only open if present.
	if ([fileManager fileExistsAtPath:path]) 
    {
		const char* cPath = [path cStringUsingEncoding:NSUTF8StringEncoding];
		FILE* file = ::fopen(cPath, flags);
		if (file) 
        {
            return file;
        }
	}
	return 0;
}

FILE* GHCocoaFileOpener::openDocumentsFile(const char* filename, const char* flags) const
{
	NSFileManager *fileManager = [NSFileManager defaultManager];
    NSString* ourDocumentsDirectory = getDocumentsPath();
	NSString* nsFilename = [[NSString alloc] initWithUTF8String:filename];
    NSString* path = [ourDocumentsDirectory stringByAppendingPathComponent:nsFilename];
	
	if (*flags == 'w' || [fileManager fileExistsAtPath:path]) 
	{
		const char* cPath = [path cStringUsingEncoding:NSUTF8StringEncoding];
		FILE* file = ::fopen(cPath, flags);
		if (file) return file;
	}
	
	return 0;
}

NSString* GHCocoaFileOpener::getDocumentsPath(void) const
{
	NSArray *paths = NSSearchPathForDirectoriesInDomains(NSApplicationSupportDirectory, NSUserDomainMask, YES);
	NSString *documentsDirectory = [paths objectAtIndex:0];
    NSString* ourDocumentsDirectory = [documentsDirectory stringByAppendingPathComponent:@"TrickShotBowling"];
    return ourDocumentsDirectory;
}

