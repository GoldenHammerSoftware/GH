// Copyright Golden Hammer Software
#if defined(GHPUTSTRINGSINTABLES) || !defined(_GHVertexComponentType_H_)
#ifndef _GHVertexComponentType_H_
#define _GHVertexComponentType_H_
#endif

#include "GHString/GHEnum.h"

// A list of stuff that can appear in a vb.
// It's a fixed list so that we can easily bind shaders to vertex components.
GHENUMBEGIN(GHVertexComponentType)
GHENUM(CT_BYTE)
GHENUM(CT_UBYTE)
GHENUM(CT_SHORT)
GHENUM(CT_FLOAT)
GHENUMEND

#include "GHString/GHEnumEnd.h"

#endif

