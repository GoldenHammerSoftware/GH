// Copyright 2010 Golden Hammer Software
#include "CommandLineParser.h"

bool CommandLineParser::getPaths(int argc, const char* argv[], NSString*& scriptPath, NSString*& scriptFilename, NSString*& destinationPath, NSArray*& sourcePaths)
{
    if (argc < 4)
    {
        NSLog(@"This program needs arguments. The first argument should be the absolute path to where you want to deposit the script. The second argument should be the destination path, relative to that path. Subsequent arguments are in-reverse-order-of-precedence the source directories, also relevant to the script path.");
        return false;
    }
    
    scriptFilename = [NSString stringWithUTF8String:argv[1]];
    destinationPath = [NSString stringWithUTF8String:argv[2]];
                       
    NSMutableArray* mutableArray = [[[NSMutableArray alloc] init] autorelease];
    for (int i = 3; i < argc; ++i)
    {
        NSString* sourcePath = [NSString stringWithUTF8String:argv[i]];
        [mutableArray addObject:sourcePath];
    }
    sourcePaths = mutableArray;
    
    
    const char* cScriptFilename = argv[1];
    const char* finalSlash = cScriptFilename + [scriptFilename length];
    while (--finalSlash > cScriptFilename)
    {
        if (*finalSlash == '/' && [scriptFilename length] - (finalSlash - cScriptFilename) > 2)
        {
            finalSlash += 2;
            char buf[[scriptFilename length]];
            snprintf(buf, finalSlash-cScriptFilename, cScriptFilename);
            buf[finalSlash-cScriptFilename] = 0;
            scriptPath = [NSString stringWithUTF8String:buf];
            return true;
        }
    }
    
    scriptPath = [NSString stringWithUTF8String:""];
    return true;
}