// Copyright Golden Hammer Software
#pragma once

#include <vector>

// Util to take strings and turn them into lists of numbers.
namespace GHNumberListParser
{
	void parseFloatList(const char* string, float* dest, unsigned int numFloats);
	void parseIntList(const char* string, int* dest, unsigned int numInts);
	void parseFloatRect(const char* string, float* ul, float* br, unsigned int numFloatsPerCorner);
	void parseIntVector(const char* string, std::vector<int>& dest);
	void parseFloatVector(const char* string, std::vector<float>& dest);
}
