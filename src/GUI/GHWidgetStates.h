// Copyright Golden Hammer Software
#if defined(GHPUTSTRINGSINTABLES) || !defined(_GHWIDGETSTATE_H_)
#ifndef _GHWIDGETSTATE_H_
#define _GHWIDGETSTATE_H_
#endif

#include "GHString/GHEnum.h"

GHENUMBEGIN(GHWidgetState)
// not visible, no input
GHENUMVAL(WS_NONE, 0)
// visible but no input
GHENUM(WS_TRANSITIONIN)
GHENUM(WS_TRANSITIONOUT)
// fully active and visible
GHENUM(WS_ACTIVE)
GHENUMEND

#include "GHString/GHEnumEnd.h"

#endif
