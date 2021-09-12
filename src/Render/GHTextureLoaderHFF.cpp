// Copyright Golden Hammer Software
#include "GHTextureLoaderHFF.h"
#include "GHPlatform/GHFileOpener.h"
#include "GHPlatform/GHFile.h"
#include "GHMemoryTexture.h"

GHTextureLoaderHFF::GHTextureLoaderHFF(const GHFileOpener& fileOpener)
: mFileOpener(fileOpener)
{
}

GHResource* GHTextureLoaderHFF::loadFile(const char* filename, GHPropertyContainer* extraData)
{
    GHMemoryTexture* ret = 0;
    
    GHFile* file = mFileOpener.openFile(filename, GHFile::FT_BINARY, GHFile::FM_READONLY);
    if (file)
    {
        size_t bufferSize = 0;
        char* bufferStart = 0;
        if (file->readIntoBuffer() && file->getFileBuffer(bufferStart, bufferSize)) {
            const char* buffer = bufferStart;
			HFFHeader header;
			readHeader(buffer, header);
            buffer += header.mDataOffset;
            
			ret = new GHMemoryTexture(header.mMapWidth, header.mMapHeight, header.mDataSize);
            void* texBuffer = 0;
			unsigned int pitch;
            ret->lockSurface(&texBuffer, pitch);
			int dataBufSize = header.mMapWidth*header.mMapHeight*header.mDataSize;
            memcpy(texBuffer, buffer, dataBufSize);
            ret->unlockSurface();
        }
        delete file;
    }
    
    return ret;
}

void GHTextureLoaderHFF::readHeader(const char* buffer, HFFHeader& header)
{
	memcpy(header.mFileTypeMarker, buffer, 4);
	buffer += 4;
	header.mBinaryMarker = *((unsigned short*)buffer);
	buffer += sizeof(unsigned short);
	memcpy(header.mTextMarker, buffer, 8);
	buffer += 8;
	header.mDataOffset = *((unsigned short*)buffer);
	buffer += sizeof(unsigned short);
	header.mMapWidth = *((unsigned int*)buffer);
	buffer += sizeof(unsigned int);
	header.mMapHeight = *((unsigned int*)buffer);
	buffer += sizeof(unsigned int);
	header.mDataSize = *buffer;
	buffer++;
	header.mFloatingPointFlag = *buffer;
	buffer++;
	header.mVerticalScale = *((float*)buffer);
	buffer += sizeof(float);
	header.mVerticalOffset = *((float*)buffer);
	buffer += sizeof(float);
	header.mHorizontalScale = *((float*)buffer);
	buffer += sizeof(float);
	header.mTileSize = *((unsigned short*)buffer);
	buffer += sizeof(unsigned short);
	header.mWrapFlag = *buffer;
}

