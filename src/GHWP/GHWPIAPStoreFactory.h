// Copyright 2010 Golden Hammer Software
#pragma once

#include "GHIAPStoreFactory.h"
#include "GHWPEventInterface.h"

class GHEventMgr;

class GHWPIAPStoreFactory : public GHIAPStoreFactory
{
public:
	GHWPIAPStoreFactory(PhoneDirect3DXamlAppComponent::GHWPEventInterface^ eventInterface,
		GHEventMgr& eventMgr);

	virtual GHIAPStore* createIAPStore(const char* configFilename, const GHXMLSerializer& xmlSerializer) const;

private:
	PhoneDirect3DXamlAppComponent::GHWPEventInterface^ mEventInterface;
	GHEventMgr& mEventMgr;
};
