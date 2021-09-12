// Copyright Golden Hammer Software
#include "GHKeyDefParser.h"
#include <string.h>

GHKeyDefParser::GHKeyDefParser(const GHIdentifierMap<int>& idMap)
: mIDMap(idMap)
{
    
}

bool GHKeyDefParser::parseString(const char* str, GHKeyDef::Enum& ret, size_t strLen) const
{
	if (strLen == 0)
	{
		strLen = strlen(str);
	}
    if (strLen == 1) {
        ret = (GHKeyDef::Enum)*str;
        return true;
    }
    else {
        GHIdentifier id = mIDMap.getHashTable().generateHash(str, strLen);
        const int* val = mIDMap.find(id);
        if (val) {
            ret = (GHKeyDef::Enum)*val;
            return true;
        }
    }
    return false;
}
