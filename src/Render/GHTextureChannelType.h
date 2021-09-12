// Copyright Golden Hammer Software
#if defined(GHPUTSTRINGSINTABLES) || !defined(_GHTextureChannelType_H_)
#ifndef _GHTextureChannelType_H_
#define _GHTextureChannelType_H_
#endif

#include "GHString/GHEnum.h"

// Types of pixel data that can be stored in a texture channel
GHENUMBEGIN(GHTextureChannelType)

GHENUM(TC_UNKNOWN) // unspecified format.

GHENUM(TC_UBYTE_NORM)
GHENUM(TC_BYTE_NORM)
GHENUM(TC_UBYTE)
GHENUM(TC_BYTE)
GHENUM(TC_USHORT_NORM)
GHENUM(TC_SHORT_NORM)
GHENUM(TC_USHORT)
GHENUM(TC_SHORT)
GHENUM(TC_UINT_NORM)
GHENUM(TC_INT_NORM)
GHENUM(TC_UINT)
GHENUM(TC_INT)
GHENUM(TC_FLOAT)
GHENUM(TC_DOUBLE)
GHENUM(TC_HALF)

GHENUMEND

#include "GHString/GHEnumEnd.h"

#endif
