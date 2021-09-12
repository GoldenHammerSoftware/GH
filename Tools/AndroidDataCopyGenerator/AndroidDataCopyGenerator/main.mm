// Copyright 2010 Golden Hammer Software
#include <iostream>
#import <Foundation/Foundation.h>
#include "AndroidDataMap.h"
#include "CommandLineParser.h"
#include "ScriptGenerator.h"

void addFilesFromPath(NSString* scriptPath, AndroidDataMap& map, NSString* sourcePath)
{
    NSFileManager *fileManager = [NSFileManager defaultManager];
    NSError* error = nil;
    
    
    NSMutableString* path = [NSMutableString stringWithString:scriptPath];
    [path appendString:sourcePath];
    NSArray* filesAtPath = [fileManager contentsOfDirectoryAtPath:path error:&error];
    
    if (error)
    {
        NSLog(@"Error getting files from path %@: %@", path, [error localizedDescription]);
        return;
    }
    
    if (!filesAtPath)
    {
        NSLog(@"contentsOfDirectoryAtPath returning nil for path %@", path);
        return;
    }
    
    NSUInteger numFiles = [filesAtPath count];
    for (NSUInteger i = 0; i < numFiles; ++i)
    {
        NSString* filename = [filesAtPath objectAtIndex:i];
        map.addEntry(sourcePath, filename);
    }
}

void constructMap(NSString* scriptPath, AndroidDataMap& map, NSArray* sourcePaths)
{
    NSUInteger numSourcePaths = [sourcePaths count];
    for (NSUInteger i = 0; i < numSourcePaths; ++i)
    {
        NSString* sourcePath = [sourcePaths objectAtIndex:i];
        addFilesFromPath(scriptPath, map, sourcePath);
    }
}

int main(int argc, const char * argv[])
{
    NSString* scriptPath;
    NSString* scriptFilename;
    NSString* destinationPath;
    NSArray* sourcePaths;
    CommandLineParser::getPaths(argc, argv, scriptPath, scriptFilename, destinationPath, sourcePaths);
    
    AndroidDataMap map;
    constructMap(scriptPath, map, sourcePaths);
    
    ScriptGenerator::generateScript(scriptFilename, map, destinationPath);

    return 0;
}

