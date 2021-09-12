// Copyright 2010 Golden Hammer Software
#pragma once

#include "GHAppleFilenameCreator.h"

class GHiOSFilenameCreator : public GHAppleFilenameCreator
{
public:
	virtual bool createFileName(const char* filename, __strong NSString*& ret);
    virtual void createWriteableFileName(const char* filename, __strong NSString*& ret);
};
