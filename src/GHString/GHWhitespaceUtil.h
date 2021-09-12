// Copyright Golden Hammer Software
#pragma once

#include <climits> //for INT_MAX

// Utility for handling whitespace within char*s.
namespace GHWhitespaceUtil
{
	bool isWhitespace(char value);
	
	//iterates over the string to find the first non-whitespace character (including NULL)
	const char* skipWhitespace(const char* start, int dir = 1, int maxIters = INT_MAX);
	char* skipWhitespace(char* start, int dir = 1, int maxIters = INT_MAX);
}
