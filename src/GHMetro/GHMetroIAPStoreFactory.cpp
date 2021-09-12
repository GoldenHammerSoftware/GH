#include "GHMetroIAPStoreFactory.h"
#include "GHMetroIAPStore.h"
#include "GHXMLNode.h"
#include "GHXMLSerializer.h"

GHIAPStore* GHMetroIAPStoreFactory::createIAPStore(const char* configFilename,
													const GHXMLSerializer& xmlSerializer) const
{
	GHMetroIAPStore* ret = new GHMetroIAPStore;
	
	GHXMLNode* node = xmlSerializer.loadXMLFile(configFilename);
	if (node) {
		for (unsigned int i = 0; i < node->getChildren().size(); ++i) {
			int id = 0;
			node->getChildren()[i]->readAttrInt("id", id);
			const char* windowsName = node->getChildren()[i]->getAttribute("windowsName");
			bool isTrialPurchase = false;
			node->getChildren()[i]->readAttrBool("isTrialPurchase", isTrialPurchase);
			ret->addItem(id, windowsName, isTrialPurchase);
		}
	}

	return ret;
}
