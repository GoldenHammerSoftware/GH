// Copyright Golden Hammer Software
#include "GHShaderPreprocessor.h"
#include "GHPlatform/GHDebugMessage.h"
#include "GHPlatform/GHFile.h"
#include "GHPlatform/GHFileOpener.h"

GHShaderPreprocessor::GHShaderPreprocessor(const GHFileOpener& fileOpener)
	: mFileOpener(fileOpener)
{
}

void GHShaderPreprocessor::preprocessShaderFile(const char* fileName, std::vector<const char*>& retGLBuffers, std::vector<char*>& retDeletionBuffers)
{
	GHFile* file = mFileOpener.openFile(fileName, GHFile::FT_TEXT, GHFile::FM_READONLY);
	if (!file) {
		// this message get spammy for the low shaders.
		//GHDebugMessage::outputString("Failed to find shader file %s", fileName);
		return;
	}

	size_t bufferSize = 0;
	char* bufferStart = 0;
	if (file->readIntoBuffer() && file->getFileBuffer(bufferStart, bufferSize))
	{
		// Claim ownership of the file buffer so we can chop it up and manage the memory.
		file->releaseFileBuffer();
		retDeletionBuffers.push_back(bufferStart);

		preprocessShaderString(fileName, bufferStart, bufferSize, retGLBuffers, retDeletionBuffers);
	}
	else
	{
		GHDebugMessage::outputString("Failed to read filebuffer for shader %s", fileName);
	}

	file->closeFile();
	delete file;
}

void GHShaderPreprocessor::preprocessShaderString(const char* fileName, char* bufferStart, size_t bufferSize, std::vector<const char*>& retGLBuffers, std::vector<char*>& retDeletionBuffers)
{
	// we expect #include "filename" if #include exists.
	char* includePos = strstr(bufferStart, "#include ");
	if (includePos != nullptr)
	{
		char* openQuotePos = includePos + 9;

		if (*openQuotePos != '"')
		{
			GHDebugMessage::outputString("ABORTING SHADER PREPROCESS #include without open quote in shader %s", fileName);
			retGLBuffers.push_back(bufferStart);
		}
		else
		{
			char* includeFileNamePos = openQuotePos + 1;

			char* endQuotePos = strchr(includeFileNamePos, '"');
			if (endQuotePos == nullptr)
			{
				GHDebugMessage::outputString("ABORTING SHADER PREPROCESS #include without close quote in shader %s", fileName);
				retGLBuffers.push_back(bufferStart);
			}
			else
			{
				// Now we know we are cutting off the original string, so \0 it at the # in #include.
				// Only push it back if it's not the first character in the string.
				if (includePos != bufferStart)
				{
					*includePos = '\0';
					retGLBuffers.push_back(bufferStart);
				}
				// cut off the file name so we can recurse.
				*endQuotePos = '\0';
				// we should have a good filename now.
				preprocessShaderFile(includeFileNamePos, retGLBuffers, retDeletionBuffers);

				// recurse to the next chunk of string.
				char* nextBufferStart = endQuotePos + 1;
				if (nextBufferStart - bufferStart < (int)bufferSize)
				{
					size_t nextBufferSize = bufferSize - (nextBufferStart - bufferStart);
					preprocessShaderString(fileName, nextBufferStart, nextBufferSize, retGLBuffers, retDeletionBuffers);
				}
			}
		}
	}
	else
	{
		// There aren't any #includes, so just push_back the whole string unmodified.
		// Assuming the string came in as null terminated here.
		retGLBuffers.push_back(bufferStart);
	}
}
