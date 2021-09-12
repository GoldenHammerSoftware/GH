// Copyright Golden Hammer Software
#include "GHWhitespaceUtil.h"
#include <cassert>

bool GHWhitespaceUtil::isWhitespace(char value)
{
	for(const char* whitespace = " \t\r\n"; *whitespace != 0; ++whitespace)	{
		if(*whitespace == value)
			return true;
	}
	return false;
}
	
//iterates over the string to find the first non-whitespace character (including NULL)
const char* GHWhitespaceUtil::skipWhitespace(const char* start, int dir, int maxIters)
{
	assert(dir == 1 || dir == -1);
	assert(maxIters > 0);
		
	//in these forbidden cases, better to fail this way than have an infinite (or very long) loop
	if(dir == 0) return start; 
	if(maxIters < 0) maxIters = -maxIters;
		
	int iters = 0;		
	while(isWhitespace(*start) && iters++ < maxIters) start += dir;
	return start;
}
    
char* GHWhitespaceUtil::skipWhitespace(char* start, int dir, int maxIters)
{
	assert(dir == 1 || dir == -1);
	assert(maxIters > 0);
		
	//in these forbidden cases, better to fail this way than have an infinite (or very long) loop
	if(dir == 0) return start; 
	if(maxIters < 0) maxIters = -maxIters;
		
	int iters = 0;		
	while(isWhitespace(*start) && iters++ < maxIters) start += dir;
	return start;
}

