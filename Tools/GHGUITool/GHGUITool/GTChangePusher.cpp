// Copyright 2010 Golden Hammer Software
#include "GTChangePusher.h"
#include "GHUtils/GHMessageHandler.h"
#include "GTMetadataList.h"
#include "GTIdentifiers.h"
#include "GTChange.h"
#include "GHUtils/GHMessage.h"

GTChangePusher::GTChangePusher(GHMessageHandler& messageHandler,
	GTMetadataList& metadataList,
	GHGUIWidgetResource* widgetResource)
	: mMessageHandler(messageHandler)
	, mMetadataList(metadataList)
	, mWidgetResource(widgetResource)
{

}

GTMetadata* GTChangePusher::createMetadataClone(void)
{
	GTMetadata* data = mMetadataList.getNode(mWidgetResource);
	return data->clone();
}

void GTChangePusher::pushChange(GTMetadata* newData)
{
	GTMetadata* oldData = mMetadataList.getNode(mWidgetResource);
	oldData = oldData->clone();

	pushChange(oldData, newData);
}

void GTChangePusher::pushChange(GTMetadata* oldData, GTMetadata* newData)
{
	//the original oldData will be deleted with this call
	if (newData) {
		mMetadataList.setNode(mWidgetResource, newData);
	}

	GHMessage pushMsg(GTIdentifiers::M_CHANGE);
	GTChange* change = new GTChange;
	pushMsg.getPayload().setProperty(GTIdentifiers::MP_CHANGE, change);

	//cloning newData and not oldData because oldData is already a clone of the one
	// removed form the metadatalist and newData
	// is not a clone of the one we just added ot the metadatalist
	change->mNewMetadata = newData ? newData->clone() : 0;
	change->mOldMetadata = oldData;
	change->mWidget = mWidgetResource;
	change->mWidget->acquire();
	mMessageHandler.handleMessage(pushMsg);
}
