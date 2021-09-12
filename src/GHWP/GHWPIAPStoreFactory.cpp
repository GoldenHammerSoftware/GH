// Copyright 2010 Golden Hammer Software
#include "GHWPIAPStoreFactory.h"
#include "GHWPIAPStore.h"
#include "GHXMLNode.h"
#include "GHXMLSerializer.h"

GHWPIAPStoreFactory::GHWPIAPStoreFactory(PhoneDirect3DXamlAppComponent::GHWPEventInterface^ eventInterface,
										 GHEventMgr& eventMgr)
: mEventInterface(eventInterface)
, mEventMgr(eventMgr)
{
}

GHIAPStore* GHWPIAPStoreFactory::createIAPStore(const char* configFilename,
													const GHXMLSerializer& xmlSerializer) const
{
	GHWPIAPStore* ret = new GHWPIAPStore(mEventInterface, mEventMgr);
	
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
