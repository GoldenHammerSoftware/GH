// Copyright Golden Hammer Software
#include "GHStringIdFactoryDJB2.h"
#include <ctype.h>

GHIdentifier GHStringIdFactoryDJB2::generateHash(const char* str, size_t len) const
{
    if (!str) return GHIdentifier(0);
    
	// algorithm from http://www.cse.yorku.ca/~oz/hash.html
	unsigned long hash = 5381;
	int c;
    const char* dummyStr = str;
    size_t numChars = 0;
	while ((c = *dummyStr++) && (len == 0 || numChars < len))
	{
		hash = ((hash << 5) + hash) + tolower(c); /* hash * 33 + c */
        numChars++;
	}
	
    GHIdentifier hashId(hash);
#ifdef DEBUG_IDENTIFIERS
    hashId.setString(str);
#endif
	return hashId;	
}
