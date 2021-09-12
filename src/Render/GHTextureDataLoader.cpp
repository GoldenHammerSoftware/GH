// Copyright Golden Hammer Software
#include "GHTextureDataLoader.h"
#include "GHTextureDataFactory.h"
#include "GHPlatform/GHFileOpener.h"
#include "GHPlatform/GHDebugMessage.h"

GHTextureDataLoader::GHTextureDataLoader(const GHFileOpener& fileOpener, const GHTextureDataFactory& dataFactory)
	: mFileOpener(fileOpener)
	, mDataFactory(dataFactory)
{
}

GHTextureData* GHTextureDataLoader::load(const char* filename) const
{
	GHFile* file = mFileOpener.openFile(filename, GHFile::FT_BINARY, GHFile::FM_READONLY);
	if (!file)
	{
		return 0;
	}

	GHTextureData* ret = 0;

	size_t bufferSize = 0;
	char* bufferStart = 0;
	if (file->readIntoBuffer() && file->getFileBuffer(bufferStart, bufferSize))
	{
		ret = mDataFactory.createFromMemory(bufferStart, bufferSize);
		if (ret)
		{
			file->releaseFileBuffer();
		}
		else
		{
			GHDebugMessage::outputString("Failed to generate texture data for %s", filename);
		}
	}
	else
	{
		GHDebugMessage::outputString("Failed to read filebuffer for texture %s", filename);
	}

	file->closeFile();
	delete file;
	return ret;
}
