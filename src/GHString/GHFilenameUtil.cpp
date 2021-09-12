#include "GHFilenameUtil.h"
#include <assert.h>
#include <string.h> // strchr
#include <stdio.h> // sprintf
#include <algorithm> // min

char* GHFilenameUtil::replaceExtension(const char* input, const char* ext)
{
	assert(input != 0);
	assert(ext != 0);

	// Slightly sloppy to avoid code duplication.
	// Create a new buffer of size input + ext + 1(\0) to avoid duplication.
	// Code that is worried about efficiency should be calling the other version of this function.
	size_t bufSize = strlen(input) + strlen(ext) + 1;
	char* ret = new char[bufSize];
	replaceExtension(input, ext, ret, bufSize);
	return ret;
}

void GHFilenameUtil::replaceExtension(const char* input, const char* ext, char* out, size_t outLen)
{
	assert(input != 0);
	assert(ext != 0);
	assert(out != 0);

	const char* existingExt = strchr(input, '.');
	size_t preExtLen = 0;
	if (existingExt)
	{
		preExtLen = existingExt - input;
	}
	else
	{
		// if we didn't find a . then append our new extension to the end of the string.
		preExtLen = strlen(input);
	}
	size_t newExtLen = strlen(ext);

	// +1 for '.'
	size_t newStrLen = preExtLen + 1 + newExtLen;
	assert(newStrLen < outLen);
	snprintf(out, std::min(preExtLen + 1, outLen), "%s", input);
	snprintf(out, outLen, "%s%s", out, ext);
	// paranoia
	out[outLen - 1] = '\0';
}
