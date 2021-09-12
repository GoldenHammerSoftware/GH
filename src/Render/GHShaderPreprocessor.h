// Copyright Golden Hammer Software
#pragma once

#include <vector>

class GHFileOpener;

class GHShaderPreprocessor
{
public:
	GHShaderPreprocessor(const GHFileOpener& fileOpener);

	// Load a file and process any #includes etc recursively and return the buffers for glShaderSource.
	// We expect the caller to need to delete the char buffers.
	void preprocessShaderFile(const char* fileName, std::vector<const char*>& retGLBuffers, std::vector<char*>& retDeletionBuffers);
	// Preprocess a piece of shader code.
	// Passing in fileName just for debug message purposes.
	void preprocessShaderString(const char* fileName, char* bufferStart, size_t bufferSize, std::vector<const char*>& retGLBuffers, std::vector<char*>& retDeletionBuffers);

private:
	const GHFileOpener& mFileOpener;
};

