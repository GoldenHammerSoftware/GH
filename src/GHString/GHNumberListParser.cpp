// Copyright Golden Hammer Software
#include "GHNumberListParser.h"
#include "GHStringTokenizer.h"

void GHNumberListParser::parseFloatList(const char* string, float* dest, unsigned int numFloats) 
{
	GHStringTokenizer strTok;
	for (unsigned int i = 0; i < numFloats; ++i) {
		dest[i] = strTok.popFloat(string, GHStringTokenizer::kWhiteTxtMarkers, string);
	}
}

void GHNumberListParser::parseIntList(const char* string, int* dest, unsigned int numInts) 
{
	GHStringTokenizer strTok;
	for (unsigned int i = 0; i < numInts; ++i) {
		dest[i] = strTok.popInt(string, GHStringTokenizer::kWhiteTxtMarkers, string);
	}
}

void GHNumberListParser::parseFloatRect(const char* string, float* ul, float* br, 
                                        unsigned int numFloatsPerCorner) 
{
	GHStringTokenizer strTok;
	for (unsigned int i = 0; i < numFloatsPerCorner; ++i)
	{
		ul[i] = strTok.popFloat(string, GHStringTokenizer::kWhiteTxtMarkers, string);
	}
	for (unsigned int i = 0; i < numFloatsPerCorner; ++i)
	{
		br[i] = strTok.popFloat(string, GHStringTokenizer::kWhiteTxtMarkers, string);
	}
}

void GHNumberListParser::parseIntVector(const char* string, std::vector<int>& dest) 
{
	GHStringTokenizer strTok;
	while (string && *string != '\0')
	{
		int val = strTok.popInt(string, GHStringTokenizer::kWhiteTxtMarkers, string);
		dest.push_back(val);
	}
}

void GHNumberListParser::parseFloatVector(const char* string, std::vector<float>& dest) 
{
	GHStringTokenizer strTok;
	while (string && *string != '\0')
	{
		float val = strTok.popFloat(string, GHStringTokenizer::kWhiteTxtMarkers, string);
		dest.push_back(val);
	}
}
