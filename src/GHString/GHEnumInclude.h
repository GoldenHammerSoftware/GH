// Copyright Golden Hammer Software

// This is the file you include in a project cpp to create the strings for an enum
// before including this file, #define ENUMHEADER "headerfilename.h"

// include once for the standard .h stuff
#include ENUMHEADER

// include a second time to declare variables.
#define GHPUTDECLARATIONS
#include ENUMHEADER
#undef GHPUTDECLARATIONS

// and include a third time to create the generate functions.
#define GHPUTSTRINGSINTABLES
#include ENUMHEADER
#undef GHPUTSTRINGSINTABLES

#undef ENUMHEADER
