// Copyright Golden Hammer Software
#if defined(GHPUTSTRINGSINTABLES) || defined(GHPUTDECLARATIONS) || !defined(_GHMESSAGETYPES_H_)
#ifndef _GHMESSAGETYPES_H_
#define _GHMESSAGETYPES_H_
#endif

#include "GHString/GHEnum.h"

GHIDENTIFIERBEGIN(GHMessageTypes)

GHIDENTIFIER(SETAPPPROPERTY)
GHIDENTIFIER(PAUSEINTERRUPT)
GHIDENTIFIER(UNPAUSEINTERRUPT)
GHIDENTIFIER(WINDOWRESIZE)
GHIDENTIFIER(INTERSTITIALADFINISHED)
GHIDENTIFIER(INTERSTITIALREWARDGRANTED)

GHIDENTIFIEREND

#include "GHString/GHEnumEnd.h"

#endif
