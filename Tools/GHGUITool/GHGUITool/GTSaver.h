// Copyright 2010 Golden Hammer Software
#pragma once

#include "GHUtils/GHMessageHandler.h"
#include "GHGUIWidget.h"
#include "GHString/GHString.h"
#include "GHMath/GHPoint.h"
#include "GTMetadataList.h"

class GHEventMgr;
class GHXMLSerializer;
class GHXMLNode;
class GHPropertyContainer;
class GHFilePicker;
class GHXMLObjFactory;
class GTUtil;

class GTSaver : public GHMessageHandler
{
public:
	GTSaver(GHEventMgr& eventMgr, const GHXMLSerializer& serializer, GTUtil& util, GHGUIWidgetResource& topLevelNode, GHPropertyContainer& props, const GHFilePicker& filePicker, const GHXMLObjFactory& objFactory, GTMetadataList& metadataList);
	~GTSaver(void);

	virtual void handleMessage(const GHMessage& message);

	void load(const char* filename);

private:
	void save(void);
	void load(void);
	GHXMLNode* createXMLNode(const GHGUIWidgetResource& widget) const;
	void fillXMLTextParameters(GHXMLNode& node, const GTMetadata::TextParameters& textParameters) const;
	void setAttribute(GHXMLNode& node, const char* attrName, GHString::CharHandlingType nameHandling, float value) const;
	void setAttribute(GHXMLNode& node, const char* attrName, GHString::CharHandlingType nameHandling, const GHPoint2& point) const;
	void setAttribute(GHXMLNode& ret, const char* attrName, GHString::CharHandlingType nameHandling, GHAlign::Enum align) const;
	void setAttribute(GHXMLNode& ret, const char* attrName, GHString::CharHandlingType nameHandling, GHFillType::Enum fillType) const;

private:
	GHEventMgr& mEventMgr;
	const GHXMLSerializer& mSerializer;
	GHGUIWidgetResource& mTopLevelNode;
	GHPropertyContainer& mProps;
	const GHFilePicker& mFilePicker;
	GTUtil& mUtil;
	GTMetadataList& mMetadataList;
};
