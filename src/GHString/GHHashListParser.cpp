// Copyright Golden Hammer Software
#include "GHHashListParser.h"
#include "GHStringTokenizer.h"
#include "GHStringIdFactory.h"

void GHHashListParser::parseHashList(const char* str, const GHStringIdFactory& hashTable,
                                     std::vector<GHIdentifier>& ret)
{
    GHStringTokenizer strTok;
    
    const char* token;
    int tokenLen;
    while (strTok.popToken(str, ' ', token, tokenLen, str))
    {
        ret.push_back(hashTable.generateHash(token, tokenLen));
    }
}

void GHHashListParser::parseHashEnumList(const char* str, const GHIdentifierMap<int>& enumStore,
                                         std::vector<int>& ret)
{
    GHStringTokenizer strTok;
    
    const char* token;
    int tokenLen;
    while (strTok.popToken(str, ' ', token, tokenLen, str))
    {
        GHIdentifier id = enumStore.getHashTable().generateHash(token, tokenLen);
        const int* val = enumStore.find(id);
        if (val) {
            ret.push_back(*val);
        }
    }
}

