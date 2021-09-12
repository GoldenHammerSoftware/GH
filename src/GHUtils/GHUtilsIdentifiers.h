// Copyright Golden Hammer Software
#if defined(GHPUTSTRINGSINTABLES) || defined(GHPUTDECLARATIONS) || !defined(_GHUTILSIDENTIFIERS_H_)
#ifndef _GHUTILSIDENTIFIERS_H_
#define _GHUTILSIDENTIFIERS_H_
#endif

#include "GHString/GHEnum.h"

GHIDENTIFIERBEGIN(GHUtilsIdentifiers)

GHIDENTIFIER(PROPS)
GHIDENTIFIER(PROPID)
GHIDENTIFIER(PROPVAL)

GHIDENTIFIER(INTVAL) // an int prop val
GHIDENTIFIER(VAL) // arbitrary val

GHIDENTIFIER(FILENAME) // name of a file with no path
GHIDENTIFIER(FILEPATH) // name and path of a file
GHIDENTIFIER(FILETOKEN) // token used by some os's for accessing a file.
GHIDENTIFIER(FILEPLATFORMDESC) // additional platform data for a file.

GHIDENTIFIER(RUNNABLE) // a runnable prop val

GHIDENTIFIEREND

#include "GHString/GHEnumEnd.h"

#endif
