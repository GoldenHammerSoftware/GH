// Copyright Golden Hammer Software
#pragma once

#include "GHMath/GHPoint.h"
#include "GHMath/GHRect.h"
#include "GHVertexStreamFormat.h"
#include <stdint.h>

class GHVertexBuffer;

// Dumping ground for handy dandy vertex buffer manipulation functions.
class GHVertexBufferUtil
{
public:
    // change the pos values of the first 4 verts to make a quad
    static void fillRect2D(float* quadStart, const GHVertexStreamFormat& streamFormat,
                           const GHPoint2& min, const GHPoint2& max);

    // change the uv values of the first 4 verts in buffer to stretch over a texture.
    static void createUVRect(GHVertexBuffer& buffer, const GHPoint2& min, const GHPoint2& max);
    static void fillUVRect(float* quadStart, const GHVertexStreamFormat& streamFormat,
                           const GHPoint2& min, const GHPoint2& max);
    
    // set numVerts worth of colors equal to textColorDword.
    static void fillColors(float* streamVerts, 
                           const GHVertexStreamFormat& streamFormat,
                           const GHVertexStreamFormat::ComponentEntry& colorCompEntry, 
                           int numVerts, int32_t textColorDword);

	// change the pos values to create a grid that fills a region
	static void fillGrid2D(float* gridStart, const GHVertexStreamFormat& streamFormat,
		                   const GHPoint2& min, const GHPoint2& max, unsigned int gridWidth, unsigned int gridHeight,
                           float angleRadians, float aspectRatio);
	// make a grid that encompasses the uv coords.
	static void createUVGrid(GHVertexBuffer& buffer, const GHPoint2& min, const GHPoint2& max, 
		unsigned int gridWidth, unsigned int gridHeight, float uvAngleInRadians);

	// clamp a uv rect to 0-1 and modify the uv coords if the source rect extended outside of the boundaries.
	static void clampToScreenEdges(const GHRect<float, 2>& posIn, const GHRect<float, 2>& uvIn, GHRect<float, 2>& posOut, GHRect<float, 2>& uvOut);

	// if a SI_TANGENT stream exists then fill it with data.
	static void populateTangents(GHVertexBuffer& vertexBuffer);
};
