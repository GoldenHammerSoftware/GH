// Copyright 2010 Golden Hammer Software
#pragma once
#include "GHGUIWidget.h"

class GHMessageHandler;
class GTMetadataList;
struct GTMetadata;

class GTChangePusher
{
public: 
	GTChangePusher(GHMessageHandler& messageHandler, 
				   GTMetadataList& metadataList,
				   GHGUIWidgetResource* resource);

	GTMetadata* createMetadataClone(void);

	void pushChange(GTMetadata* newData);
	void pushChange(GTMetadata* oldData, GTMetadata* newData);

private:
	GHMessageHandler& mMessageHandler;
	GTMetadataList& mMetadataList;
	GHGUIWidgetResource* mWidgetResource;
};
