#include "GHTexture.h"

void* GHTexture::getPixel(unsigned int col, unsigned int row, void* lockedSurface)
{
	unsigned int width = 0, height = 0, depth = 0;
	if (!getDimensions(width, height, depth))
    { 
        return 0; 
    }
    
	unsigned int idx = depth*col + row*width*depth;
	if (idx > width*height*depth)
    { 
        return 0; 
    }
    
    char* surfaceAsChars = (char*)lockedSurface;
    return (void*)&surfaceAsChars[idx];
}
