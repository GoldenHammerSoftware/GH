// Copyright 2010 Golden Hammer Software
#include "GTMetadataList.h"
#include "GHUtils/GHEventMgr.h"
#include "GTMetadataList.h"
#include "GTIdentifiers.h"
#include "GHUtils/GHMessage.h"
#include "GHGUIWidget.h"
#include "GHXMLNode.h"

GTMetadata* GTMetadata::clone(void)
{
	GTMetadata* newData = new GTMetadata(mName, mParentName, mPosDesc, mRenderableNode ? mRenderableNode->clone() : 0, mText, mTextParameters);
	return newData;
}

GTMetadata::~GTMetadata(void)
{
	delete mRenderableNode;
}

GTMetadataList::~GTMetadataList(void)
{
	DataMap::iterator iter = mMap.begin(), iterEnd = mMap.end();
	for (; iter != iterEnd; ++iter)
	{
		delete iter->second;
	}
}

GTMetadata* GTMetadataList::getNode(GHGUIWidgetResource* widget)
{
	DataMap::iterator iter = mMap.find(widget);
	if (iter != mMap.end())
	{
		return iter->second;
	}
	else
	{
		return 0;
	}
}

const GTMetadata* GTMetadataList::getNode(const GHGUIWidgetResource* widget) const
{
	DataMap::const_iterator iter = mMap.find(const_cast<GHGUIWidgetResource*>(widget));
	if (iter != mMap.end())
	{
		return iter->second;
	}
	else
	{
		return 0;
	}
}

void GTMetadataList::setNode(GHGUIWidgetResource* widget, GTMetadata* metadata)
{
	DataMap::iterator iter = mMap.find(widget);
	if (iter != mMap.end())
	{
		delete iter->second;
		iter->second = metadata;
	}
	else
	{
		mMap.insert(DataMap::value_type(widget, metadata));
	}
}


