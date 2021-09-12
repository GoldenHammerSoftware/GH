// Copyright 2010 Golden Hammer Software
#if defined(GHPUTSTRINGSINTABLES) || !defined(_CGGAMESTATES_H_)
#ifndef _CGGAMESTATES_H_
#define _CGGAMESTATES_H_
#endif

#include "GHEnum.h"

GHENUMBEGIN(CGGameStates)
// Menu States
GHENUM(GS_PAUSE)
GHENUM(GS_DEBUG)
GHENUM(GS_MENU)
GHENUM(GS_GAME)

// Game input states, second state machine
GHENUM(GS_DEFAULT)
GHENUM(GS_PLACING)

GHENUMEND

#include "GHEnumEnd.h"

#endif
