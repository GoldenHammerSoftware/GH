// Copyright Golden Hammer Software
#include "GHDebugMessage.h"
#include "GHOutputPipe.h"
#include <stdarg.h>
#include <stdio.h>

GHOutputPipe* GHDebugMessage::sOutput = 0;

void GHDebugMessage::init(GHOutputPipe& output)
{
	sOutput = &output;
}

void GHDebugMessage::outputString(const char* str, ...)
{
	if (!sOutput || !str) return;
	
	{
		//scoping so buffer doesn't allocate if string is empty
		//if messages are getting cut off, increase this buffer size
		const int bufSize = 8192;
		char buffer[bufSize];
		va_list args;
		va_start(args, str);
		vsnprintf(buffer, bufSize, str, args);
		va_end(args);
		sOutput->outputString(buffer);
	}
}

void GHDebugMessage::outputPreformattedString(const char* str)
{
	if (!sOutput || !str) return;
	sOutput->outputString(str);
}
