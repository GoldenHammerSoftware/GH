// Copyright 2010 Golden Hammer Software
#pragma once
#import <Foundation/Foundation.h>

//Specific to AndroidDataCopyGenerator: takes c-style argc, argv. Converts them
// to a destination path and list of source paths
class CommandLineParser
{
public:
    static bool getPaths(int argc, const char* argv[], NSString*& scriptPath, NSString*& scriptFilename, NSString*& destinationPath, NSArray*& sourcePaths);
};