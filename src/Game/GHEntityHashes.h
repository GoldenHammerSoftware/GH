// Copyright Golden Hammer Software
#if defined(GHPUTSTRINGSINTABLES) || defined(GHPUTDECLARATIONS) || !defined(_GHENTITYHASHES_H_)
#ifndef _GHENTITYHASHES_H_
#define _GHENTITYHASHES_H_
#endif

#include "GHString/GHEnum.h"

GHIDENTIFIERBEGIN(GHEntityHashes)
// standard active state for ents.
GHIDENTIFIER(ES_ACTIVE)
// standard not active state for ents.
GHIDENTIFIER(ES_INACTIVE)

// command to load an entity.  expects a P_ENTFILE and an optional P_ENTID
GHIDENTIFIER(M_LOADENTITY)
// command to kill an entity with a P_ENTID
GHIDENTIFIER(M_KILLENTITY)
// notification that an entity specified by prop ENTITY was created.
GHIDENTIFIER(M_ENTITYCREATED)
// notification that an entity specified by prop ENTITY was deleted.
GHIDENTIFIER(M_ENTITYDELETED)

// standard property for a entity's singular model
GHIDENTIFIER(MODEL)
// a GHEntity* in a property container
GHIDENTIFIER(P_ENTITY)
// the file used as source for an ent, ghstring prop
GHIDENTIFIER(P_ENTFILE)
// entity identifier unique to all ents in the world.
GHIDENTIFIER(P_ENTID)
//used for recursive loading: the physics object being loaded (not necessarily the one directly on the model)
GHIDENTIFIER(P_PHYSICSOBJECT)
GHIDENTIFIEREND

#include "GHString/GHEnumEnd.h"

#endif
