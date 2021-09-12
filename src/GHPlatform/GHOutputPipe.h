// Copyright Golden Hammer Software
#pragma once

// Interface class for outputting a string to some target
class GHOutputPipe
{
public:
	virtual ~GHOutputPipe(void) {}
	
	virtual void outputString(const char* str) = 0;
};
