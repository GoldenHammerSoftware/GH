// Copyright Golden Hammer Software
#pragma once

class GHVBBlitterIndex;

// Dumping ground for handy dandy index buffer manipulation functions.
class GHIndexBufferUtil
{
public:
    static unsigned short* insertTriangleIndices(unsigned short* currIndex, 
                                                 unsigned short val1, unsigned short val2, 
                                                 unsigned short val3);
    
    // createQuad takes the corners of the quad as arguments.
    static unsigned short* createQuad(unsigned short* currIndex,
                                      unsigned short val0, unsigned short val1, 
                                      unsigned short val2, unsigned short val3);
    
	static void fillIndexBufferForQuads(GHVBBlitterIndex& buffer, unsigned int numQuads, bool isStrip=false);
    static void fillIndexBufferForGrid(GHVBBlitterIndex& buffer, unsigned int width, unsigned int height);
};
