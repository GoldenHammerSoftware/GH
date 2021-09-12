// Copyright 2010 Golden Hammer Software
#include "AndroidDataMap.h"


bool AndroidDataMap::StringWrapper::operator<(const StringWrapper& other) const
{
    NSComparisonResult result = [mStr compare:other.mStr];
    return result < NSOrderedSame;
}

bool AndroidDataMap::StringWrapper::operator==(const StringWrapper& other) const
{
    return [mStr isEqualToString:other.mStr];
}

void AndroidDataMap::addEntry(NSString* sourcePath, NSString* filename)
{
    if (!entryAllowed(filename)) {
        return;
    }
    
    StringWrapper key(filename);
    mEntries[key] = sourcePath;
}

bool AndroidDataMap::entryAllowed(const NSString* filename) const
{
    static NSString* disallowedList[] = {
        @".DS_Store",
        @".svn",
        @".pvr",
        @".DDS",
        @".hlsl"
    }; 
    
    for (int i = 0; i < sizeof(disallowedList)/sizeof(disallowedList[0]); ++i)
    {
        NSRange range = [filename rangeOfString:disallowedList[i]];
        if (range.location != NSNotFound) {
            return false;
        }
    }
    return true;
}
