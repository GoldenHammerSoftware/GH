// Copyright Golden Hammer Software
#pragma once

#include "GHMath/GHPoint.h"
#include <stdint.h>

struct GHMD3ModelHeader 
{
	// note: the order of this struct has to match up with the file format.
	int32_t magic;
	int32_t version;
	char name[64];
	int32_t flags;
    
	int32_t numframes;
	int32_t numtags;
	int32_t numsurfaces;
	int32_t numskins;
    
	int32_t framesoffset;
	int32_t tagsoffset;
	int32_t surfacesoffset;
	int32_t eofoffset;
};

struct GHMD3SurfaceDescription
{
	int32_t ident;
	char surfacename[64];
	int32_t flags;
	int32_t numframes;
	int32_t numshaders;
	int32_t numverts;
	int32_t numtriangles;
	int32_t trianglesoffset;
	int32_t shadersoffset;
	int32_t stoffset;
	int32_t vertexoffset;
	int32_t end;
};

struct GHMD3Frame 
{
	GHPoint3 minBounds;
	GHPoint3 maxBounds;
	GHPoint3 localOrigin;
	float radius;
	char name[16];
};
