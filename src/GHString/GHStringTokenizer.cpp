// Copyright Golden Hammer Software
#include "GHStringTokenizer.h"
#include <memory.h>
#include <string.h>
#include <stdlib.h>

const char* GHStringTokenizer::kWhiteTxtMarkers = " \r\n\t";

static __inline const char* findNextMarker( const char* searchbuffer, const char* markers )
{
	for( ; *searchbuffer; ++searchbuffer )
	{
		for( int m = 0; markers[m]; ++m )
		{
			if( *searchbuffer == markers[m] )
			{
				return searchbuffer;
			}
		}
	}
	return NULL;
}

bool GHStringTokenizer::popToken(char* buffer, char marker, char*& token, 
                                 char*& rebuffer, const char* max) const
{
	const char markers[2] = { marker, 0 };
	return popToken(buffer, markers, token, rebuffer, max);
}

bool GHStringTokenizer::popToken(char* buffer, const char* markers, char*& token,
                                 char*& rebuffer, const char* max) const
{
	int tokenLen = 0;
	if (!popToken(buffer, markers, (const char*&)token, tokenLen, (const char*&)rebuffer, max)) {
		return false;
	}
	char* tokenEnd = token + tokenLen;
	*tokenEnd = '\0';
	return true;
}

bool GHStringTokenizer::popToken(const char* buffer, char marker, const char*& token, 
                                 int& tokenLen, const char*& rebuffer, const char* max) const
{
    if (!buffer) return false;
	const char markers[2] = { marker, 0 };
	return popToken(buffer, markers, token, tokenLen, rebuffer, max);
}

bool GHStringTokenizer::popToken(const char* buffer, const char* markers, const char*& token, 
								 int& tokenLen, const char*& rebuffer, const char* max) const
{
	if(!*buffer)
		return false;
	
	// find the first instance of the marker char within the buffer that isn't within quotes.
	int len = (int)::strlen(buffer);
	// bool to say we should exit our while loop.
	bool foundToken = false;
	// bool to say we found a quote, and need to find the next quote before continuing.
	bool foundQuote = false;
	// current index into the main buffer so we can keep track of where we left off.
	const char* searchbuffer = buffer;
	// pointer to the first instance of the token in the string.
	const char* tokenMarker = 0;
	
	while (!foundToken) {
		// base state, we havn't encountered an open quote yet.
		if (!foundQuote) {
			// find a marker for first token.
			const char* markerSearch = findNextMarker(searchbuffer, markers);
			
			if (!markerSearch) {
				// no instance of the marker, so return that the entire buffer is the token.
				token = buffer;
				tokenLen = len;
				rebuffer = buffer + len;
				return true;
			}
			// we found a potential token, so set out pointer.
			tokenMarker = markerSearch;
			int markerLen = (int)(markerSearch - searchbuffer);
			
			// find the first instance of '"' for comparison.
			const char* quotePtr = ::strchr(searchbuffer, (int)'"');
			if (quotePtr) {
				// see if the quote or the marker comes first.
				int quoteLen = (int)(quotePtr - searchbuffer);
				if (quoteLen >= markerLen) {
					// the token comes first, so use it.
					foundToken = true;
				} else {
					// found an open quote before hitting the marker, so need to look for an endquote before continuing.
					// make sure we don't crash on a bad file.
					if (len <= quoteLen) {
						// we hit a lack of the marker within our buffer.
						return false;
					}
					// switch to the secondary state with a new starting point.
					searchbuffer = quotePtr+1;
					foundQuote = true;
				}
			} else {
				// no quote, so use what we have.
				foundToken = true;
			}
		} else {
			// secondary state, we've encountered a quote and need to find an endquote.
			const char* quotePtr = ::strchr(searchbuffer, (int)'"');
			if (quotePtr) {
				// see if the quote or the marker comes first.
				int quoteLen = (int)(quotePtr - buffer);
				if (quoteLen > len) {
					// we hit an open quote without an end quote.
					return false;
				} else {
					searchbuffer = quotePtr+1;
					foundQuote = false;
				}
			}
		}
	}
	if (!tokenMarker) {
		return false;
	}
	
	if (max && tokenMarker > max) {
		return false;
	}
	tokenLen = (int)(tokenMarker - buffer);
	if (tokenLen <= (int)len) {
		// point the next buffer at the string after our token.
		rebuffer = tokenMarker+1;
	} else {
		rebuffer = 0;
	}
	token = buffer;
	return true;
}

int GHStringTokenizer::popInt(const char* buffer, char marker, const char*& rebuffer) const
{
	const char markers[2] = { marker, 0 };
	return popInt(buffer, markers, rebuffer);
}

int GHStringTokenizer::popInt(const char* buffer, const char* markers, 
                              const char*& rebuffer) const
{
	int tokenLen = 0;
	const char* strVal = 0;
	if (GHStringTokenizer::popToken(buffer, markers, strVal, tokenLen, rebuffer, 0)) {
		char temp[64];
		if (tokenLen > 63) tokenLen = 63;
		::strncpy(temp, buffer, tokenLen);
		temp[tokenLen] = '\0';
		return ::atoi(buffer);
	}
	else {
		return ::atoi(buffer);
	}
}

float GHStringTokenizer::popFloat(const char* buffer, char marker, 
                                  const char*& rebuffer) const
{
	const char markers[2] = { marker, 0 };
	return popFloat(buffer, markers, rebuffer);
}

float GHStringTokenizer::popFloat(const char* buffer, const char* markers, 
                                  const char*& rebuffer) const
{
	int tokenLen = 0;
	const char* strVal = 0;
	if (GHStringTokenizer::popToken(buffer, markers, strVal, tokenLen, rebuffer, 0)) {
		char temp[64];
		if (tokenLen > 63) tokenLen = 63;
		::strncpy(temp, buffer, tokenLen);
		temp[tokenLen] = '\0';
		return (float)::atof(buffer);
	}
	else {
		return (float)::atof(buffer);
	}
	return true;
}

bool GHStringTokenizer::popBool(const char* buffer, const char* markers, 
                                const char*& rebuffer) const
{
	int tokenLen = 0;
	const char* strVal = 0;
	if(GHStringTokenizer::popToken(buffer, markers, strVal, tokenLen, rebuffer, 0)) 
	{
		const char* falseValues[] = {
			"false",
			"False",
			"FALSE",
			"0"
		};
		
		size_t numFalseVals = sizeof(falseValues)/sizeof(falseValues[0]);
		for(size_t i = 0; i < numFalseVals; ++i)	{
			if(!strcmp(strVal, falseValues[i]))
				return false;
		}
		return true;
	}
	return false;
}

