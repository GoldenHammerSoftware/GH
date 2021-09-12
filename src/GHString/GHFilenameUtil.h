#pragma once

#include <stddef.h> // size_t

// A place for common filename manipulation functions.
namespace GHFilenameUtil
{
	// Replace any existing extension or add a new one if not found.
	// Return new memory.
	char* replaceExtension(const char* input, const char* ext);
	// Replace any existing extension or add a new one if not found.
	// Copy result into out.
	void replaceExtension(const char* input, const char* ext, char* out, size_t outLen);
}
