// Copyright 2010 Golden Hammer Software
#include "GHiOSAVFSoundLoader.h"
#include <AVFoundation/AVFoundation.h>
#include "GHAVFSoundHandle.h"
#include "GHAVPSoundHandle.h"
#include "GHUtils/GHPropertyContainer.h"
#include "GHUtils/GHUtilsIdentifiers.h"
#import <MediaPlayer/MediaPlayer.h>
#include "GHPlatform/GHDebugMessage.h"

GHiOSAVFSoundLoader::GHiOSAVFSoundLoader(void)
{

}

GHResource* GHiOSAVFSoundLoader::loadFile(const char* filename, GHPropertyContainer* extraData)
{
    NSURL* url = 0;
    const char* filekey = 0;
    if (extraData)
    {
        filekey = extraData->getProperty(GHUtilsIdentifiers::FILETOKEN);
        if (filekey)
        {
            url = [NSURL URLWithString:[NSString stringWithUTF8String:filekey]];
        }
    }

    if (!url)
    {
        NSString* path;
        if (!mFileNameCreator.createFileName(filename, path)) {
            return 0;
        }
        url = [NSURL fileURLWithPath: path];
    }
    
    GHSoundHandle* ret = new GHAVPSoundHandle(url);
    extractMetaData(*ret, filename);
    return ret;
}

void GHiOSAVFSoundLoader::extractMetaData(GHSoundHandle& ret, const char* filename)
{
    // remove the .mp3 to get the MPMediaItemPersistantKey
    size_t oldLen = strlen(filename);
    char* newName = new char[oldLen];
    snprintf(newName, oldLen-3, "%s", filename);

    // get the MPMediaItem so we can read the properties.
    unsigned long long ullvalue = strtoull(newName, NULL, 0);
    NSNumber* numberID = [[NSNumber alloc] initWithUnsignedLongLong:ullvalue];
    MPMediaPropertyPredicate* predicate = [MPMediaPropertyPredicate predicateWithValue:numberID forProperty:MPMediaItemPropertyPersistentID];

    MPMediaQuery* mediaQuery = [[MPMediaQuery alloc] init];
    [mediaQuery addFilterPredicate:predicate];
    NSArray *items = [mediaQuery items];
//    for (int i = 0; i < items.count; ++i)
    if (items.count > 0)
    {
        int i = 0; // artifact of the loop approach.

        MPMediaItem* songItem = [items objectAtIndex:i];
        NSNumber* id = [songItem valueForProperty:MPMediaItemPropertyPersistentID];
        NSURL* url = [songItem valueForProperty:MPMediaItemPropertyAssetURL];
        NSString* artist = [songItem valueForProperty:MPMediaItemPropertyArtist];
        NSString* album = [songItem valueForProperty:MPMediaItemPropertyAlbumTitle];
        NSString* songName = [songItem valueForProperty:MPMediaItemPropertyTitle];

        ret.mIDInfo.setAlbumName(GHString([album UTF8String], GHString::CHT_COPY));
        ret.mIDInfo.setSongName(GHString([songName UTF8String], GHString::CHT_COPY));
        ret.mIDInfo.setArtistName(GHString([artist UTF8String], GHString::CHT_COPY));
        
        // save the album art to a file so we can load it through the rendering code.
        MPMediaItemArtwork *itemArtwork = [songItem valueForProperty:MPMediaItemPropertyArtwork];
        if (itemArtwork)
        {
            const int preferredArtSize = 256;
            UIImage* artworkUIImage = [itemArtwork imageWithSize:CGSizeMake(preferredArtSize, preferredArtSize)];
            char imgName[2048];
            snprintf(imgName, 2048, "%s_%s_%s.png", [artist UTF8String], [songName UTF8String], [album UTF8String]);
            NSString* imgPath;
            mFileNameCreator.createWriteableFileName(imgName, imgPath);
            
            // the image might not actually be a power of 2, we need to resize it if so.
            UIGraphicsBeginImageContext(CGSizeMake(preferredArtSize,preferredArtSize));
            [artworkUIImage drawInRect:CGRectMake(0,0,preferredArtSize,preferredArtSize)];
            UIImage* resizedImage = UIGraphicsGetImageFromCurrentImageContext();
            UIGraphicsEndImageContext();
            
            [UIImagePNGRepresentation(resizedImage) writeToFile:imgPath atomically:YES];
            ret.mIDInfo.setPictureName(imgName);
        }
        else {
            GHDebugMessage::outputString("Failed to find album art.");
        }
    }

    
    delete [] newName;
}
