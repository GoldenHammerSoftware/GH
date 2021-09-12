// Copyright Golden Hammer Software
#pragma once

#include "GHKeyDef.h"

// dumping ground for useful char functions
class GHCharUtil
{
public:
	// take the lower case key, and if shiftHeld then turn it into an upper case.
	// 1 becomes !, which toUpper does not do.
	static GHKeyDef::Enum applyUpperCase(GHKeyDef::Enum input, bool shiftHeld);
};
