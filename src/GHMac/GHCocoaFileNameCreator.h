// Copyright 2010 Golden Hammer Software
#pragma once

#include "GHAppleFilenameCreator.h"

// util class to figure out Cocoa file loading arguments
class GHCocoaFileNameCreator : public GHAppleFilenameCreator
{
public:
	GHCocoaFileNameCreator(void);
    ~GHCocoaFileNameCreator(void);
	
	virtual bool createFileName(const char* filename, __strong NSString*& ret) override;
    virtual void createWriteableFileName(const char* filename, __strong NSString*& ret) override;

private:
	NSFileManager* mFileManager;
	NSString* mDocumentsDirectory;

};
