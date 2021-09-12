// Copyright Golden Hammer Software
#if defined(GHPUTSTRINGSINTABLES) || !defined(_GHHFUVTYPE_H)
#ifndef _GHHFUVTYPE_H
#define _GHHFUVTYPE_H
#endif

#include "GHString/GHEnum.h"

// type of uv to create for a height field
GHENUMBEGIN(GHHFUVType)
GHENUM(HFUV_NONE) // no uv coords
GHENUM(HFUV_PROJECT) // top down projection of coords
GHENUM(HFUV_CLIFF) // de-stretch the uv for cliffs as much as possible
GHENUMEND

#include "GHString/GHEnumEnd.h"

#endif
