// Copyright 2010 Golden Hammer Software
#pragma once

class GHAppleFilenameCreator 
{
public:
    virtual ~GHAppleFilenameCreator(void) { }
    // look for an existing file
	virtual bool createFileName(const char* filename, __strong NSString*& ret) = 0;
    // create a name for a file that can be created (documents dir)
    virtual void createWriteableFileName(const char* filename, __strong NSString*& ret) = 0;
};
