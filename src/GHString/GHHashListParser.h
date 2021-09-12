// Copyright Golden Hammer Software
#pragma once

#include <vector>
#include "GHIdentifier.h"
#include "GHIdentifierMap.h"

class GHStringIdFactory;

// Utility for turning a space separated list into a vector of string hashes
namespace GHHashListParser
{
    void parseHashList(const char* str, const GHStringIdFactory& hashTable, 
		std::vector<GHIdentifier>& ret);
    void parseHashEnumList(const char* str, const GHIdentifierMap<int>& enumStore, 
		std::vector<int>& ret);
}
