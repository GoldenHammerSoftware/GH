// Copyright 2010 Golden Hammer Software
#if defined(GHPUTSTRINGSINTABLES) || !defined(_SBUPDATEPRIORITY_H_)
#ifndef _MCUPDATEPRIORITY_H_
#define _MCUPDATEPRIORITY_H_
#endif

#include "GHString/GHEnum.h"

GHENUMBEGIN(MCUpdatePriority)
GHENUMVAL(UP_TIME,-5)
GHENUMVAL(UP_PHYSICS,-3)
GHENUM(UP_DEFAULT)
GHENUM(UP_RENDER)
GHENUMEND

#include "GHString/GHEnumEnd.h"

#endif
