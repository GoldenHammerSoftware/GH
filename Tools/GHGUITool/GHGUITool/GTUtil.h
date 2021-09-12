// Copyright 2010 Golden Hammer Software
#pragma once
#include "GHGUIWidget.h"
#include "GHString/GHString.h"
#include "GTMetadataList.h"

class GHXMLNode;
class GHXMLObjFactory;
class GHStringIdFactory;
class GTChangePusher;

class GTUtil
{
public:
	GTUtil(const GHXMLObjFactory& xmlObjFactory, const GHStringIdFactory& idFactory, GHGUIWidgetResource& topPanel, GTMetadataList& metadataList);

	void setRandomColor(GHGUIWidgetResource& widget);
	void setColor(GHGUIWidgetResource& widget, const GHPoint4& color, GTChangePusher* changePusher=0);
	void setTexture(GHGUIWidgetResource& widget, const char* filename, GTChangePusher& changePusher);
	void setMaterial(GHGUIWidgetResource& widget, GHXMLNode* matNode, GTChangePusher* changePusher=0);
	bool setUVAngle(GHGUIWidgetResource& widget, float uvAngle, GTChangePusher& changePusher);
	void setRenderableNode(GHGUIWidgetResource& widget, const GHXMLNode& renderableNode, GTChangePusher* changePusher);
	void setText(GHGUIWidgetResource& widget, const char* text, GTChangePusher* changePusher);
	void setTextParameters(GHGUIWidgetResource& widget, const GTMetadata::TextParameters& textParameters, GTChangePusher* changePusher);
	bool getTextParameters(const GHGUIWidgetResource& widget, GTMetadata::TextParameters& outTextParameters, const char** outText = 0);

	const GHIdentifier& getIdForAllTexts(void) const { return mIDForAllTexts; }

	static GHXMLNode* createEmptyPanelNode(void);
	static GHXMLNode* createColorMatNode(const GHPoint4& color);
	static GHXMLNode* createTextureMatNode(const char* texFile, GHString::CharHandlingType texFileHandlingType);

	static GHGUIWidgetResource* findParent(GHGUIWidgetResource& potentialParent, const GHGUIWidgetResource& target);

	GHGUIWidgetResource* createPanel(void);
	GHGUIWidgetResource* createPanel(const GHXMLNode& node, const GHIdentifier& parentId);
	GHGUIWidgetResource* createText(void);
	GHGUIWidgetResource* createSanitizedTextChild(const GHXMLNode& node);

	GHGUIWidgetResource* getTextChild(GHGUIWidgetResource& widget) const;
	const GHGUIWidgetResource* getTextChild(const GHGUIWidgetResource& widget) const;

private:
	const GHXMLObjFactory& mXMLObjFactory;
	const GHStringIdFactory& mIdFactory;
	GHGUIWidgetResource& mTopPanel;
	GTMetadataList& mMetadataList;
	GHIdentifier mIDForAllTexts;
};