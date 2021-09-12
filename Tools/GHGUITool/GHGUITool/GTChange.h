// Copyright 2010 Golden Hammer Software
#pragma once
#include "GHString/GHString.h"
#include "GHGUIWidget.h"
#include "GTMetadataList.h"

//If creation, set mNewMetadata but not mOldMetadata
//If deletion, set mOldMetadata, but not mNewMetadata
//Any other change, set entirety of both mOldMetadata and mNewMetadata, even if redundant.
// The diff will determine what changed.
struct GTChange
{
	GHGUIWidgetResource* mWidget;
	GTMetadata* mOldMetadata;
	GTMetadata* mNewMetadata;

	GTChange(void) : mWidget(0), mOldMetadata(0), mNewMetadata(0) { }
	~GTChange(void) 
	{
		if (mWidget) mWidget->release();
		delete mOldMetadata;
		delete mNewMetadata;
	}
};