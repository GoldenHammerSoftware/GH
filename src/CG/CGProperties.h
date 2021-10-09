// Copyright 2010 Golden Hammer Software
#if defined(GHPUTSTRINGSINTABLES) || defined(GHPUTDECLARATIONS) || !defined(_CG_PROPERTIES_H_)
#ifndef _CG_PROPERTIES_H_
#define _CG_PROPERTIES_H_
#endif

#include "GHEnum.h"

GHIDENTIFIERBEGIN(CGProperties)
GHIDENTIFIER(PP_DESTINATION)
GHIDENTIFIER(PP_HASDESTINATION)
GHIDENTIFIER(PP_ISSELECTED)
GHIDENTIFIER(PP_TEAM)
GHIDENTIFIER(PP_MOVERADIUS)
GHIDENTIFIER(PP_INITIATIVEPOINTS)
GHIDENTIFIER(PP_HITPOINTS)
GHIDENTIFIER(PP_ATTACKTARGET)
GHIDENTIFIER(PP_ISATTACKING)
GHIDENTIFIER(PP_DEFAULTMOVERADIUS)
GHIDENTIFIER(PP_DEFAULTINITIATIVEPOINTS)
GHIDENTIFIER(PP_DEFAULTHITPOINTS)
GHIDENTIFIER(PP_ATTACKRADIUS)
GHIDENTIFIER(PP_ATTACKDAMAGE)
GHIDENTIFIER(GP_MOVECIRCLE)
GHIDENTIFIER(GP_SELECTEDNAME)
GHIDENTIFIER(GP_SELECTEDCOST)
GHIDENTIFIER(GP_ACTIVEPLAYERNAME)
GHIDENTIFIER(GP_CARDNAME0)
GHIDENTIFIER(GP_CARDNAME1)
GHIDENTIFIER(GP_CARDNAME2)
GHIDENTIFIER(GP_CARDNAME3)
GHIDENTIFIER(GP_CARDNAME4)
GHIDENTIFIER(GP_CARDSREMAINING)
GHIDENTIFIER(PP_ENTCOLOR);
GHIDENTIFIER(PP_DIEIMMEDIATELY)
GHIDENTIFIER(GP_MANA)
GHIDENTIFIER(GP_MAXMANA)
GHIDENTIFIER(PP_BASE)

//messages
GHIDENTIFIER(CHANGEMAINSTATE)
GHIDENTIFIER(NEWGAME)
GHIDENTIFIER(EXITGAME)
GHIDENTIFIER(SELECTCARD)
GHIDENTIFIER(INDEX)
GHIDENTIFIER(ENDTURN)
GHIDENTIFIER(UPGRADEBASE)

GHIDENTIFIEREND

#include "GHEnumEnd.h"

#endif
