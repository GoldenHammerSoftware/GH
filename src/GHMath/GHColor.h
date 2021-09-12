// Copyright Golden Hammer Software
#pragma once

#include "GHPoint.h"
#include <stdint.h>

// util for treating point4s as colors.
class GHColor
{
public:
	static int32_t convertToDword(const GHPoint4& color)
	{
		unsigned int retval = 0;
		
		unsigned char red = (unsigned char)(color[0]*255);
		unsigned char green = (unsigned char)(color[1]*255);
		unsigned char blue = (unsigned char)(color[2]*255);
		unsigned char alpha = (unsigned char)(color[3]*255);
		unsigned char* currByte = (unsigned char*)(&retval);
		*currByte = red; currByte++;
		*currByte = green; currByte++;
		*currByte = blue; currByte++;
		*currByte = alpha; 
		
		return retval;
	}
    
	static int32_t convertToDword(const GHPoint3& color)
	{
		unsigned int retval = 0;
		
		unsigned char red = (unsigned char)(color[0]*255);
		unsigned char green = (unsigned char)(color[1]*255);
		unsigned char blue = (unsigned char)(color[2]*255);
		unsigned char alpha = (unsigned char)(255);
		unsigned char* currByte = (unsigned char*)(&retval);
		*currByte = red; currByte++;
		*currByte = green; currByte++;
		*currByte = blue; currByte++;
		*currByte = alpha;
		
		return retval;
	}
    
};
