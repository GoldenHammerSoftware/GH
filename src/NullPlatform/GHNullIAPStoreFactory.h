// Copyright Golden Hammer Software
#pragma once

#include "Game/GHIAPStoreFactory.h"
#include "NullPlatform/GHNullIAPStore.h"

// Creates a null in-app purchase store
/*
	<stuff>
		<blah id=1/>
		<blah id=2/>
	</stuff>
*/
class GHNullIAPStoreFactory : public GHIAPStoreFactory
{
public:
	GHNullIAPStoreFactory(bool alwaysBuy);

    virtual GHIAPStore* createIAPStore(const char* configFilename,
                                       const GHXMLSerializer& xmlSerializer) const;

private:
	bool mPurchaseResult;
};
