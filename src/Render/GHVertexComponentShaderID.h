// Copyright Golden Hammer Software
#if defined(GHPUTSTRINGSINTABLES) || !defined(_GHVertexComponentShaderID_H_)
#ifndef _GHVertexComponentShaderID_H_
#define _GHVertexComponentShaderID_H_
#endif

#include "GHString/GHEnum.h"

// A list of stuff that can appear in a vb.
// It's a fixed list so that we can easily bind shaders to vertex components.
GHENUMBEGIN(GHVertexComponentShaderID)
GHENUMVAL(SI_POS, 0)
GHENUMVAL(SI_NORMAL, 1)
GHENUMVAL(SI_TANGENT, 2)
GHENUMVAL(SI_DIFFUSE, 3)
GHENUMVAL(SI_SPECULAR, 4)
GHENUMVAL(SI_BONEIDX, 5)
GHENUMVAL(SI_BONEWEIGHT, 6)
GHENUMVAL(SI_UV1, 7)
GHENUMVAL(SI_UV2, 8)
GHENUMVAL(SI_MAX,9)
GHENUMEND

#include "GHString/GHEnumEnd.h"

#endif

