// Copyright Golden Hammer Software
#include "GHNullIAPStoreFactory.h"
#include "GHNullIAPStore.h"
#include "Base/GHXMLNode.h"
#include "Base/GHXMLSerializer.h"

GHNullIAPStoreFactory::GHNullIAPStoreFactory(bool alwaysBuy)
: mPurchaseResult(alwaysBuy)
{
}

GHIAPStore* GHNullIAPStoreFactory::createIAPStore(const char* configFilename, 
												  const GHXMLSerializer& xmlSerializer) const
{
	GHNullIAPStore* ret = new GHNullIAPStore(mPurchaseResult);
	
	GHXMLNode* node = xmlSerializer.loadXMLFile(configFilename);
	if (node) {
		for (unsigned int i = 0; i < node->getChildren().size(); ++i) {
			int id = 0;
			node->getChildren()[i]->readAttrInt("id", id);
			ret->addItem(id);
		}
	}

	return ret;
}
