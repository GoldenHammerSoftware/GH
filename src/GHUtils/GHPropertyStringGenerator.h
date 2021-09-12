// Copyright Golden Hammer Software
#pragma once

#include "GHString/GHIdentifier.h"
#include <vector>
#include "GHString/GHString.h"

class GHPropertyContainer;

// widget to do a printf type string creation based on properties.
class GHPropertyStringGenerator
{
public:
    GHPropertyStringGenerator(const GHPropertyContainer& propertyContainer,
                              unsigned int outputBufferSize);
    ~GHPropertyStringGenerator(void);
    
    const char* generateString(void);
    
    // for loading.
    void addMonitoredProperty(const GHIdentifier& propId);
    void setSourceString(const char* str);
    
private:
    const GHPropertyContainer& mPropertyContainer;
    
    // our source format string, similar to what we'd pass to printf.
    GHString mSourceString;
    unsigned int mBufferSize;
    // a buffer to hold what we will return in generateString.
    char* mOutputBuffer;
    // a data buffer we use while creating the outputbuffer.
    char* mScratchBuffer;
    
    // list of property ids we care about
    std::vector<GHIdentifier> mPropertyTypes;
    // the list of indices into mPropertyTypes in order of insertion into the string.
	std::vector<int> mOrderedPropertyTypes;
	// the list of insertion tags (%s, %d, etc) for those property values.
	std::vector<char> mPropertyInsertionTags;
	// a list of strings to insert in between property values.
	std::vector<char*> mPrefixInsertionStrings;
};
