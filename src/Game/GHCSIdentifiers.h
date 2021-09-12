// Copyright Golden Hammer Software
#if defined(GHPUTSTRINGSINTABLES) || defined(GHPUTDECLARATIONS) || !defined(_GHCSIDENTIFIERS_H_)
#ifndef _GHCSIDENTIFIERS_H_
#define _GHCSIDENTIFIERS_H_
#endif

#include "GHString/GHEnum.h"

GHIDENTIFIERBEGIN(GHCSIdentifiers)
// start a local game
GHIDENTIFIER(M_STARTLOCAL)
// disconnect our client
GHIDENTIFIER(M_DISCONNECT)
// set the level that will be used by our local server
//  if we have one running or will run one eventually.
GHIDENTIFIER(M_SETLOCALLEVEL)
// argument to M_SETLOCALLEVEL
GHIDENTIFIER(P_LEVELNAME)
GHIDENTIFIEREND

#include "GHString/GHEnumEnd.h"

#endif
