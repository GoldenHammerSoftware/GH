// Copyright 2010 Golden Hammer Software
#include "GHWPSoundLoader.h"
#include "GHFileOpener.h"
#include "GHDeletionHandle.h"
#include "GHDebugMessage.h"
#include "GHMetroSoundHandle.h"

GHWPSoundLoader::GHWPSoundLoader(GHFileOpener& fileOpener)
: mFileOpener(fileOpener)
{
}

#ifndef MAKEFOURCC
    #define MAKEFOURCC(ch0, ch1, ch2, ch3)                              \
                ((uint32)(byte)(ch0) | ((uint32)(byte)(ch1) << 8) |       \
                ((uint32)(byte)(ch2) << 16) | ((uint32)(byte)(ch3) << 24 ))
#endif /* defined(MAKEFOURCC) */

bool readChunk(const char* dataPtr, uint32 length, uint32& offset, DWORD fourcc, DWORD& outChunkSize, DWORD& outChunkPos)
{
	while (true)
	{
		if (offset + sizeof(DWORD)*2 >= length) 
		{
			return false;
		}

		DWORD chunkType = *reinterpret_cast<const DWORD*>(&dataPtr[offset]);
		DWORD chunkSize = *reinterpret_cast<const DWORD*>(&dataPtr[offset + sizeof(DWORD)]);
		offset += sizeof(DWORD)*2;

		if (chunkType == MAKEFOURCC('R', 'I', 'F', 'F'))
		{
			outChunkSize = sizeof(DWORD);
		}
		else
		{
			outChunkSize = chunkSize;
		}

		outChunkPos = offset;
		offset += outChunkSize;

		if (chunkType == fourcc)
		{
			return true;
		}
	}
}

GHResource* GHWPSoundLoader::loadFile(const char* filename, GHPropertyContainer* extraData)
{
	GHFile* file = mFileOpener.openFile(filename, GHFile::FT_BINARY, GHFile::FM_READONLY);
	if (!file) {
		return 0;
	}
	GHTypedDeletionHandle<GHFile> fileDeletor(file);
	if (!file->readIntoBuffer()) {
		return 0;
	}
	char* fileBuffer = 0;
	size_t bufferSize = 0;
	if (!file->getFileBuffer(fileBuffer, bufferSize))
	{
		return 0;
	}

	DWORD chunkSize = 0, chunkPos = 0;

	uint32 offset = 0;
	if (!readChunk(fileBuffer, bufferSize, offset,
		MAKEFOURCC('R', 'I', 'F', 'F'),  chunkSize, chunkPos)) {
		GHDebugMessage::outputString("Failed to find RIFF chunk in %s", filename);
		return 0;
	}
	if (*reinterpret_cast<const DWORD*>(&fileBuffer[chunkPos]) != MAKEFOURCC('W', 'A', 'V', 'E')) {
		GHDebugMessage::outputString("RIFF chunk in %s does not say WAVE", filename);
		return 0;
	}

	if(!readChunk(fileBuffer, bufferSize, offset,
		MAKEFOURCC('f', 'm', 't', ' '), chunkSize, chunkPos)) {
			GHDebugMessage::outputString("Failed to find fmt chunk in %s", filename);
			return 0;
	}
	if (!(chunkSize <= sizeof(WAVEFORMATEX))) {
		GHDebugMessage::outputString("Format chunk size is too big in %s", filename);
		return 0;
	}
	int formatChunkPos = chunkPos;
	int formatChunkSize = chunkSize;
			
	if(!readChunk(fileBuffer, bufferSize, offset,
		MAKEFOURCC('d', 'a', 't', 'a'), chunkSize, chunkPos)) {
			GHDebugMessage::outputString("Failed to find data chunk in %s", filename);
			return 0;
	}
	GHMetroSoundHandle* ret = new GHMetroSoundHandle;
	::memcpy_s(&ret->mFormat, sizeof(ret->mFormat), &fileBuffer[formatChunkPos], formatChunkSize);
	ret->mPlayData = ref new Platform::Array<byte>(chunkSize);
	::memcpy_s(ret->mPlayData->Data, chunkSize, &fileBuffer[chunkPos], chunkSize);
	return ret;
}
