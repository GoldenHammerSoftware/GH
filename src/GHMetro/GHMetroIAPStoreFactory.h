#pragma once

#include "GHIAPStoreFactory.h"

class GHMetroIAPStoreFactory : public GHIAPStoreFactory
{
public:
	virtual GHIAPStore* createIAPStore(const char* configFilename, const GHXMLSerializer& xmlSerializer) const;
};
