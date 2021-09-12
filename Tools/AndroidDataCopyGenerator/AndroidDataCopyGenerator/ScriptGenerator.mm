// Copyright 2010 Golden Hammer Software
#include "ScriptGenerator.h"
#include "AndroidDataMap.h"
#include <stdio.h>
#include <iostream>

static const char* calcExtension(const char* filename)
{
    const char* extension = filename;
    while (*extension != 0 && *extension != '.') {
        ++extension;
    }
    
    if (extension == 0) {
        return "";
    }
    
    
    static const char* kNoCompressExt[] = {
        ".jpg", ".jpeg", ".png", ".gif",
        ".wav", ".mp2", ".mp3", ".ogg", ".aac",
        ".mpg", ".mpeg", ".mid", ".midi", ".smf", ".jet",
        ".rtttl", ".imy", ".xmf", ".mp4", ".m4a",
        ".m4v", ".3gp", ".3gpp", ".3g2", ".3gpp2",
        ".amr", ".awb", ".wma", ".wmv"
    };
    
    for (int i = 0; i < sizeof(kNoCompressExt)/sizeof(kNoCompressExt[0]); ++i)
    {
        if (!strcmp(extension, kNoCompressExt[i])) {
            return "";
        }
    }
    
    return ".mp3";
}

static void writeEntry(NSString* srcPath, NSString* filename, NSString* destPath, FILE* file)
{
    //all the extra stuff we're putting in the line on top of the input strings
    const char moreData[] = "cp /.mp3 / \r\n";
    
    char buffer[[srcPath length] + [filename length]*2 + [destPath length] + sizeof(moreData)];
    
    const char* cSrcPath = [srcPath cStringUsingEncoding:NSUTF8StringEncoding];
    const char* cFilename = [filename cStringUsingEncoding:NSUTF8StringEncoding];
    const char* cDestPath = [destPath cStringUsingEncoding:NSUTF8StringEncoding];
    
    //append slashes to the paths if and only if they weren't added already.
    const char* srcSlash = *(cSrcPath+[srcPath length]-1) == '/' ? "" : "/";
    const char* destSlash = *(cDestPath+[destPath length]-1) == '/' ? "" : "/";
    
    const char* destExtension = calcExtension(cFilename);
    
    int totalStringLength = snprintf(buffer, sizeof(buffer), "cp %s%s%s %s%s%s%s\n", cSrcPath, srcSlash, cFilename, cDestPath, destSlash, cFilename, destExtension);    
    fwrite(buffer, 1, totalStringLength, file);
}

void ScriptGenerator::generateScript(NSString* scriptFilename, AndroidDataMap& map, NSString* destinationDir)
{
    FILE* file = ::fopen([scriptFilename cStringUsingEncoding:NSUTF8StringEncoding], "wt");
    if (!file)
    {
        NSLog(@"Failed to open shell script file %@ for write.", scriptFilename);
        return;
    }
    
    AndroidDataMap::EntryMap::iterator iter = map.mEntries.begin(), iterEnd = map.mEntries.end();
    for (; iter != iterEnd; ++iter)
    {
        writeEntry(iter->second, iter->first.mStr, destinationDir, file);
    }
    
    fclose(file);
    
    NSLog(@"Script generated at %@", scriptFilename);
}