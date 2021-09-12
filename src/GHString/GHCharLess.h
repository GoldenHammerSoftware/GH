// Copyright Golden Hammer Software
#pragma once

#include <string.h>

// Struct for safely using char*'s in stl structs.
struct GHCharLess
{
public:
	bool operator()(const char* str1, const char* str2) const 
	{ 
		if (!str1) return false;
		if (!str2) return false;
		size_t len1, len2;
		len1 = ::strlen(str1);
		len2 = ::strlen(str2);
		return compare(str1, len1, str2, len2);
	}
	
	bool compare(const char* str1, size_t len1, const char* str2, size_t len2) const
	{
		size_t minLen = len1;
		if (len2 < minLen) minLen = len2;

		// compare characters up to min len looking for differences.
		for (size_t i = 0; i < minLen; ++i)
		{
			if (str1[i] != str2[i]) {
				return str1[i] < str2[i];
			}
		}
		// str1 or str2 is a substr of the other.  if one is shorter pick that one.
		if (len1 != len2) {
			// different length chars so they can't be equal.
			return (len1 < len2);
		}
		// strings are identical, so neither is less than the other.
		return false;
	}
};
