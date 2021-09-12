// Copyright 2010 Golden Hammer Software
#pragma once
#import <Foundation/Foundation.h>

class AndroidDataMap;

//Input:
// -an AndroidDataMap containing the source directories and filenames
// -an output directory
//
//Generates a shell script that copies the files to the output directory.
// Also responsible for appending ".mp3" if necessary
class ScriptGenerator
{
public:
    static void generateScript(NSString* scriptFilename, AndroidDataMap& map, NSString* destinationDir);
};