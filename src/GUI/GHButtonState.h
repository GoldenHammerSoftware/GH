// Copyright Golden Hammer Software
#if defined(GHPUTSTRINGSINTABLES) || !defined(_GHBUTTONSTATE_H_)
#ifndef _GHBUTTONSTATE_H_
#define _GHBUTTONSTATE_H_
#endif

#include "GHString/GHEnum.h"

GHENUMBEGIN(GHButtonState)
GHENUMVAL(BE_DISABLED, 0) // visible but no input
GHENUM(BE_IDLE) // visible with input and no mouseover
GHENUM(BE_SELECTED) // mouseover
GHENUM(BE_TRIGGERED) // clicked
GHENUM(BE_MAX)
GHENUMEND

#include "GHString/GHEnumEnd.h"

#endif
