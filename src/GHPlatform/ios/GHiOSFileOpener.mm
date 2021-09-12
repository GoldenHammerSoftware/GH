// Copyright 2010 Golden Hammer Software
#include "GHiOSFileOpener.h"
#include "GHFileC.h"

GHFile* GHiOSFileOpener::openPlatformFile(const char* filename, 
                                          GHFile::FileType fileType, 
                                          GHFile::FileMode fileMode) const
{
	if (!filename) return 0;
    
    const char* flags = "r";
    if (fileMode == GHFile::FM_READONLY)
    {
        if (fileType == GHFile::FT_BINARY) flags = "rb";
    }
    else
    {
        if (fileType == GHFile::FT_BINARY) flags = "wb";
        else flags = "w";
    }
    
    
	// check documents directory first, failing that check the bundle.
	FILE* ret = openDocumentsFile(filename, flags);
	if (!ret) {
		ret = openBundleFile(filename, flags);
	}
    if (!ret) return 0;
	return new GHFileC(ret, fileType, fileMode);
}

FILE* GHiOSFileOpener::openBundleFile(const char* filename, const char* flags) const
{
	NSFileManager *fileManager = [NSFileManager defaultManager];
	NSString* documentsDirectory = [[NSBundle mainBundle] bundlePath];
    
	NSString* nsFilename = 0;
    nsFilename = [[NSString alloc] initWithUTF8String:filename];
    NSString* path = [documentsDirectory stringByAppendingPathComponent:nsFilename];
	
	// can't open write files in the bundle directory, so only open if present.
	if ([fileManager fileExistsAtPath:path]) {
        const char* iosCPath = [fileManager fileSystemRepresentationWithPath: path];
		FILE* file = ::fopen(iosCPath, flags);
		if (file) return file;
	}
	return 0;
}

FILE* GHiOSFileOpener::openDocumentsFile(const char* filename, const char* flags) const
{
	NSFileManager *fileManager = [NSFileManager defaultManager];
    
	NSArray *paths = NSSearchPathForDirectoriesInDomains(NSDocumentDirectory, NSUserDomainMask, YES);
	NSString *documentsDirectory = [paths objectAtIndex:0];
	NSString* nsFilename = [[NSString alloc] initWithUTF8String:filename];
    NSString* path = [documentsDirectory stringByAppendingPathComponent:nsFilename];
    
	if (*flags == 'w' || [fileManager fileExistsAtPath:path]) 
	{
        const char* iosCPath = [fileManager fileSystemRepresentationWithPath: path];
		FILE* file = ::fopen(iosCPath, flags);
		if (file) return file;
	}
	return 0;
}
