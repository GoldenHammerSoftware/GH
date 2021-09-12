// Copyright 2010 Golden Hammer Software
#include "GHPVRUtil.h"
#include "GHPlatform/GHFileOpener.h"
#include "GHiOSFilenameCreator.h"
#include "GHPlatform/GHDebugMessage.h"

#define PVR_TEXTURE_FLAG_TYPE_MASK    0xff
static char gPVRTexIdentifier[5] = "PVR!";

bool GHPVRUtil::unpackData(void* data, size_t dataSize, PVRTextureDesc& tex,
                           int rgba_4b_enum, int rgb_4b_enum, 
                           int rgba_2b_enum, int rgb_2b_enum)
{
    bool success = false;
    PVRTexHeader *header = NULL;
    uint32_t flags, pvrTag;
    uint32_t dataLength = 0, dataOffset = 0;
    uint32_t blockSize = 0, widthBlocks = 0, heightBlocks = 0;
    uint32_t width = 0, height = 0, bpp = 4;
    uint8_t *bytes = NULL;
    uint32_t formatFlags;
    
    header = (PVRTexHeader *)data;
    
    pvrTag = CFSwapInt32LittleToHost(header->pvrTag);
	
    if (gPVRTexIdentifier[0] != ((pvrTag >>  0) & 0xff) ||
        gPVRTexIdentifier[1] != ((pvrTag >>  8) & 0xff) ||
        gPVRTexIdentifier[2] != ((pvrTag >> 16) & 0xff) ||
        gPVRTexIdentifier[3] != ((pvrTag >> 24) & 0xff))
    {
        GHDebugMessage::outputString("Unsupported gPVRTexIdentifier");
        return false;
    }
    
    flags = CFSwapInt32LittleToHost(header->flags);
    formatFlags = flags & PVR_TEXTURE_FLAG_TYPE_MASK;
    
    if (formatFlags == kPVRTextureFlagTypePVRTC_4 || formatFlags == kPVRTextureFlagTypePVRTC_2)
    {
        tex._imageData.clear();
        
        tex._width = width = CFSwapInt32LittleToHost(header->width);
        tex._height = height = CFSwapInt32LittleToHost(header->height);
        
        if (CFSwapInt32LittleToHost(header->bitmaskAlpha))
            tex._hasAlpha = TRUE;
        else
            tex._hasAlpha = FALSE;
        
        if (formatFlags == kPVRTextureFlagTypePVRTC_4) {
			if (tex._hasAlpha == TRUE) {
				tex._internalFormat = rgba_4b_enum;
			} else {
				tex._internalFormat = rgb_4b_enum;
			}
		}
        else if (formatFlags == kPVRTextureFlagTypePVRTC_2) {
			if (tex._hasAlpha == TRUE) {
				tex._internalFormat = rgba_2b_enum;
			} else {
				tex._internalFormat = rgb_2b_enum;
			}
		}
		
        dataLength = CFSwapInt32LittleToHost(header->dataLength);
        
        bytes = ((uint8_t *)data) + sizeof(PVRTexHeader);
        
        // Calculate the data size for each texture level and respect the minimum number of blocks
		int numLevels = 0;
        uint32_t mipSize = 0;
        while (dataOffset < dataLength)
        {
			numLevels++;
            if (formatFlags == kPVRTextureFlagTypePVRTC_4)
            {
                blockSize = 4 * 4; // Pixel by pixel block size for 4bpp
                widthBlocks = width / 4;
                heightBlocks = height / 4;
                bpp = 4;
            }
            else
            {
                blockSize = 8 * 4; // Pixel by pixel block size for 2bpp
                widthBlocks = width / 8;
                heightBlocks = height / 4;
                bpp = 2;
            }
            
            // Clamp to minimum number of blocks
            if (widthBlocks < 2)
                widthBlocks = 2;
            if (heightBlocks < 2)
                heightBlocks = 2;
			
            mipSize = widthBlocks * heightBlocks * ((blockSize  * bpp) / 8);
            
            tex._imageData.push_back(bytes+dataOffset);
            tex._imageDataSize.push_back(mipSize);
            
            dataOffset += mipSize;
            
            width = MAX(width >> 1, 1);
            height = MAX(height >> 1, 1);
        }
		if (numLevels > 1) tex._hasMips = TRUE;
		else tex._hasMips = FALSE;
		
        success = true;
    }
    else {
        GHDebugMessage::outputString("Unsupported PVR_TEXTURE_FLAG_TYPE_MASK");
    }
    
	return success;
}

bool GHPVRUtil::loadPVRData(const char* filename, GHiOSFilenameCreator& fileNameCreator,
                               const GHFileOpener& fOpener, void*& data, size_t& dataSize)
{
    GHFile* pvrFile = fOpener.openFile(filename, GHFile::FT_BINARY, GHFile::FM_READONLY);
    if (!pvrFile)
    {
        return false;
    }

    char* fileData = 0;
    if (pvrFile->readIntoBuffer() && pvrFile->getFileBuffer(fileData, dataSize)) { }
    else
    {
        GHDebugMessage::outputString("Failed to read filebuffer for pvr texture %s", filename);
        pvrFile->closeFile();
        delete pvrFile;
        return false;
    }
    data = fileData;
    pvrFile->releaseFileBuffer();
    return true;
}
