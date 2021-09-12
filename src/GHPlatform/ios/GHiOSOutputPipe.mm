// Copyright 2010 Golden Hammer Software
#include "GHiOSOutputPipe.h"

void GHiOSOutputPipe::outputString(const char* str)
{
#if 1
	NSString* outStr = [[NSString alloc] initWithUTF8String:str];
	NSLog(@"%@", outStr);
#endif    
}
