// Copyright Golden Hammer Software
#pragma once

class GHIAPStore;
class GHXMLSerializer;

//An interface for a factory that creates
// in-app purchase stores.
class GHIAPStoreFactory
{
public:
    virtual ~GHIAPStoreFactory(void) { }
    
    virtual GHIAPStore* createIAPStore(const char* configFilename,
                                       const GHXMLSerializer& xmlSerializer) const = 0;
};
