// Copyright 2010 Golden Hammer Software
#if defined(GHPUTSTRINGSINTABLES) || defined(GHPUTDECLARATIONS) || !defined(_GHRAGDOLLTOOLCOLLISIONLAYERS_H_)
#ifndef _GHRAGDOLLTOOLCOLLISIONLAYERS_H_
#define _GHRAGDOLLTOOLCOLLISIONLAYERS_H_
#endif

#include "GHEnum.h"

//***************************************
// NOTE:
//***************************************
// These are power-of-two bitflags that
//  must fit in a 16-bit value.
// This is also sort of Bullet-specific,
//  since Bullet reserves the first seven
//  values (and also the final one). We are
//  defining those values identically to the
//  Bullet ones for compatibility.
//***************************************

GHENUMBEGIN(GHRagdollToolCollisionLayers)

//Bullet vals
GHENUMVAL(NOCOLLISION, 0)
GHENUMVAL(DEFAULTCOLLISION, 1)
GHENUMVAL(KINEMATICCOLLISION, 4)
GHENUMVAL(DEBRISCOLLISION, 8)
GHENUMVAL(SENSORTRIGGERCOLLISION, 16)
GHENUMVAL(CHARACTERCOLLISION, 32)
//end of Bullet vals


GHENUMVAL(ALLCOLLISION, ~0) //Bullet val

GHENUMEND

#endif