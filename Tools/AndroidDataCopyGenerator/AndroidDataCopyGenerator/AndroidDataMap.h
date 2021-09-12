// Copyright 2010 Golden Hammer Software
#pragma once
#include <map>
#import <Foundation/NSString.h>

//An aggregation of all the mappings from source filename to destination filename, complete with path.
// * Removes duplicates on add
// * Filters out certain files eg, .svn folders
class AndroidDataMap
{
public:
    //If the filename already exists, this method will replace the previous sourcePath with the new one.
    void addEntry(NSString* sourcePath, NSString* filename);
    
    struct StringWrapper
    {
        StringWrapper(NSString* str) : mStr(str) { }
        NSString* mStr;
        
        bool operator<(const StringWrapper& other) const;
        bool operator==(const StringWrapper& other) const;
    };

    typedef std::map<StringWrapper, NSString*> EntryMap;
    EntryMap mEntries;
    
private:
    bool entryAllowed(const NSString* filename) const;
};