// Copyright Golden Hammer Software
#pragma once

#include "GHString/GHIdentifierMap.h"
#include "GHKeyDef.h"

class GHKeyDefParser
{
public:
    GHKeyDefParser(const GHIdentifierMap<int>& idMap);
    
	bool parseString(const char* str, GHKeyDef::Enum& ret, size_t strLen=0) const;
    
private:
    const GHIdentifierMap<int>& mIDMap;
};
