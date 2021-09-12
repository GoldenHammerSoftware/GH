// Copyright Golden Hammer Software
#include "GHIndexBufferUtil.h"
#include "GHVBBlitter.h"
#include "GHPlatform/GHDebugMessage.h"

unsigned short* GHIndexBufferUtil::insertTriangleIndices(unsigned short* currIndex, unsigned short val1, 
                                                         unsigned short val2, unsigned short val3)
{
	*currIndex = val1;
	currIndex++;
	*currIndex = val2;
	currIndex++;
	*currIndex = val3;
	currIndex++;
	return currIndex;
}

unsigned short* GHIndexBufferUtil::createQuad(unsigned short* currIndex, unsigned short val0, 
                                              unsigned short val1, unsigned short val2, 
                                              unsigned short val3)
{
	currIndex = insertTriangleIndices(currIndex, val0, val1, val3);
	currIndex = insertTriangleIndices(currIndex, val0, val3, val2);
	return currIndex;
}

void GHIndexBufferUtil::fillIndexBufferForQuads(GHVBBlitterIndex& buffer, unsigned int numQuads, bool isStrip)
{
	unsigned short* ibuf = (unsigned short*)(buffer.lockWriteBuffer());
	if (!ibuf) {
		GHDebugMessage::outputString("fillIndexBufferForQuads: Unable to lock buffer index.\n");
		return;
	}
	// sanity check buffer size.
	if (buffer.getNumIndices() < numQuads*2*3) {
		GHDebugMessage::outputString("fillIndexBufferForQuads: buffer too small.\n");
		return;
	}
	
	unsigned int quadStart = 0;
	for (unsigned int i = 0; i < numQuads; ++i) {
		ibuf = insertTriangleIndices(ibuf, quadStart+2, quadStart, quadStart+1);
		ibuf = insertTriangleIndices(ibuf, quadStart+2, quadStart+1, quadStart+3);
		quadStart += 4;
		if (isStrip) {
			quadStart -= 2; // strips share edges.
		}
	}
	
	buffer.unlockWriteBuffer();
}

void GHIndexBufferUtil::fillIndexBufferForGrid(GHVBBlitterIndex& buffer, unsigned int width, unsigned int height)
{
	unsigned short* currIndex = (unsigned short*)(buffer.lockWriteBuffer());
	if (!currIndex) {
		return;
	}
	unsigned int rowIdx = 0;
	unsigned int nextRowIdx = width;
	for (unsigned int row = 0; row < height-1; ++row) {
		for (unsigned int col = 0; col < width-1; ++col) {
			currIndex = insertTriangleIndices(currIndex, nextRowIdx+col, 
				rowIdx + col, rowIdx + col + 1);
			
			currIndex = insertTriangleIndices(currIndex, nextRowIdx+col+1, 
				nextRowIdx + col, rowIdx + col + 1);
		}
		rowIdx += width;
		nextRowIdx += width;
	}
	buffer.unlockWriteBuffer();
}

