// Copyright Golden Hammer Software
#pragma once

// A class to break up strings into token separated lists for parsing.
class GHStringTokenizer
{
public:
	// Null terminated array of usual white text markers.
	static const char* kWhiteTxtMarkers;
	
	// take a buffer, find the first token, pop it from the buffer.
	// ...call the const version and make the end of the token '\0'
	bool popToken(char* buffer, char marker, char*& token,				// Single marker
				  char*& rebuffer, const char* max=0) const;
	bool popToken(char* buffer, const char* markers, char*& token,		// Multiple markers
				  char*& rebuffer, const char* max=0) const;
	
	// pop token without changing the initial string.
	// substrings with "" are treated as atoms.
	bool popToken(const char* buffer, char marker,						// Single marker
				  const char*& token, int& tokenLen, const char*& rebuffer, 
				  const char* max=0) const;
	
	// pop token without changing the initial string.
	// substrings with "" are treated as atoms.
	bool popToken(const char* buffer, const char* markers,				// Multiple markers
				  const char*& token, int& tokenLen, const char*& rebuffer, 
				  const char* max=0) const;
	
	int popInt(const char* buffer, const char* markers, const char*& rebuffer) const;
	int popInt(const char* buffer, char marker, const char*& rebuffer) const;
	float popFloat(const char* buffer, const char* markers, const char*& rebuffer) const;
	float popFloat(const char* buffer, char marker, const char*& rebuffer) const;
	bool popBool(const char* buffer, const char* markers, const char*& rebuffer) const;	
};
