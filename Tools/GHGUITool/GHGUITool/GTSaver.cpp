// Copyright 2010 Golden Hammer Software
#include "GTSaver.h"
#include "GHUtils/GHEventMgr.h"
#include "GHXMLNode.h"
#include "GHXMLSerializer.h"
#include "GTIdentifiers.h"
#include "GHUtils/GHPropertyContainer.h"
#include "GHUtils/GHMessage.h"
#include "GHPlatform/GHFilePicker.h"
#include "GHUtils/GHUtilsIdentifiers.h"
#include "GHXMLObjFactory.h"
#include "GTUtil.h"
#include "GTMetadataList.h"

GTSaver::GTSaver(GHEventMgr& eventMgr, const GHXMLSerializer& serializer, GTUtil& util, GHGUIWidgetResource& topLevelNode, GHPropertyContainer& props, const GHFilePicker& filePicker, const GHXMLObjFactory& objFactory, GTMetadataList& metadataList)
	: mEventMgr(eventMgr)
	, mTopLevelNode(topLevelNode)
	, mSerializer(serializer)
	, mProps(props)
	, mFilePicker(filePicker)
	, mUtil(util)
	, mMetadataList(metadataList)
{
	mEventMgr.registerListener(GTIdentifiers::M_SAVE, *this);
	mEventMgr.registerListener(GTIdentifiers::M_LOAD, *this);
}

GTSaver::~GTSaver(void)
{
	mEventMgr.unregisterListener(GTIdentifiers::M_LOAD, *this);
	mEventMgr.unregisterListener(GTIdentifiers::M_SAVE, *this);
}

void GTSaver::handleMessage(const GHMessage& message)
{
	if (message.getType() == GTIdentifiers::M_SAVE)
	{
		save();
	}
	else if (message.getType() == GTIdentifiers::M_LOAD)
	{
		load();
	}
}

void GTSaver::save(void)
{
	GHXMLNode* node = createXMLNode(mTopLevelNode);
	bool result = mSerializer.writeXMLFile("guitoolresult.xml", *node);
	delete node;

	if (result) {
		mProps.setProperty(GTIdentifiers::GP_CONSOLERESULTSTR, "Saved guitoolresult.xml");
	}
	else {
		mProps.setProperty(GTIdentifiers::GP_CONSOLERESULTSTR, "Failed to save guitoolresult.xml");
	}
}

void GTSaver::load(void)
{
	class Callback : public GHFilePicker::PickedCallback
	{
	public:
		Callback(GTSaver& loader) : mLoader(loader) { }
		virtual void handleFilePicked(PickedCallback::Result& result)
		{
			mLoader.load(result.mFilePath);
		}
	private:
		GTSaver& mLoader;
	};

	std::vector<const char*> exts;
	exts.push_back(".xml");
	mFilePicker.pickFile(Callback(*this), exts);
}

void GTSaver::load(const char* filename)
{
	GHXMLNode* node = mSerializer.loadXMLFile(filename);
	if (!node) { return; }

	GHGUIWidgetResource* topNode = mUtil.createPanel(*node, GHIdentifier());

	while (topNode->get()->getNumChildren())
	{
		GHGUIWidgetResource* child = topNode->get()->getChildAtIndex(0);
		child->acquire();
		topNode->get()->removeChild(child);
		mTopLevelNode.get()->addChild(child);
		child->get()->updatePosition();
		child->release();
	}


	delete node;
}

void GTSaver::fillXMLTextParameters(GHXMLNode& ret, const GTMetadata::TextParameters& textParameters) const
{
	ret.setAttribute("font", GHString::CHT_REFERENCE, "ericfont.xml", GHString::CHT_REFERENCE);
	ret.setAttribute("wrap", GHString::CHT_REFERENCE, textParameters.mWrap ? "true" : "false", GHString::CHT_REFERENCE);

	setAttribute(ret, "textHeight", GHString::CHT_REFERENCE, textParameters.mHeight);
	setAttribute(ret, "fill", GHString::CHT_REFERENCE, textParameters.mFill);
	setAttribute(ret, "hAlign", GHString::CHT_REFERENCE, textParameters.mHAlign);
	setAttribute(ret, "vAlign", GHString::CHT_REFERENCE, textParameters.mVAlign);
}

GHXMLNode* GTSaver::createXMLNode(const GHGUIWidgetResource& widget) const
{
	GTMetadata::TextParameters textParameters;
	const char* textChars = 0;
	bool isText = mUtil.getTextParameters(widget, textParameters, &textChars);

	GHXMLNode* ret = new GHXMLNode;
	if (isText)
	{
		ret->setName("guiText", GHString::CHT_REFERENCE);
		ret->setAttribute("text", GHString::CHT_REFERENCE, textChars, GHString::CHT_REFERENCE);
		fillXMLTextParameters(*ret, textParameters);
	}
	else
	{
		ret->setName("guiPanel", GHString::CHT_REFERENCE);
	}

	ret->setAttribute("id", GHString::CHT_REFERENCE, widget.get()->getId().getString(), GHString::CHT_REFERENCE);

	GHXMLNode* posNode = new GHXMLNode;
	posNode->setName("guiPos", GHString::CHT_REFERENCE);
	ret->addChild(posNode);
	const GHGUIPosDesc& posDesc = widget.get()->getPosDesc();

	setAttribute(*posNode, "xFill", GHString::CHT_REFERENCE, posDesc.mXFill);
	setAttribute(*posNode, "yFill", GHString::CHT_REFERENCE, posDesc.mYFill);

	setAttribute(*posNode, "xSizeAlign", GHString::CHT_REFERENCE, posDesc.mSizeAlign[0]);
	setAttribute(*posNode, "ySizealign", GHString::CHT_REFERENCE, posDesc.mSizeAlign[1]);

	setAttribute(*posNode, "align", GHString::CHT_REFERENCE, posDesc.mAlign);
	setAttribute(*posNode, "offset", GHString::CHT_REFERENCE, posDesc.mOffset);
	setAttribute(*posNode, "size", GHString::CHT_REFERENCE, posDesc.mSize);

	const GTMetadata* metadata = mMetadataList.getNode(&widget);
	const GHXMLNode* renderableNode = metadata ? metadata->mRenderableNode : 0;
	if (renderableNode)
	{
		GHXMLNode* clonedRenderableNode = renderableNode->clone();
		GHXMLNode* renderableParentNode = new GHXMLNode;
		renderableParentNode->setName("renderable", GHString::CHT_REFERENCE);
		renderableParentNode->addChild(clonedRenderableNode);
		ret->addChild(renderableParentNode);
	}

	size_t numChildren = widget.get()->getNumChildren();

	if (numChildren)
	{
		GHXMLNode* childrenNode = new GHXMLNode;
		childrenNode->setName("children", GHString::CHT_REFERENCE);
		ret->addChild(childrenNode);
		for (size_t i = 0; i < numChildren; ++i)
		{
			const GHGUIWidgetResource* child = widget.get()->getChildAtIndex(i);
			if (child->get()->getId() == mUtil.getIdForAllTexts())
			{
				continue;
			}

			GHXMLNode* childNode = createXMLNode(*child);
			childrenNode->addChild(childNode);
		}
	}

	return ret;
}

void GTSaver::setAttribute(GHXMLNode& node, const char* attrName, GHString::CharHandlingType nameHandling, float value) const
{
	const int BUFSZ = 256;
	char buffer[BUFSZ];
	snprintf(buffer, BUFSIZ, "%f", value);
	node.setAttribute(attrName, nameHandling, buffer, GHString::CHT_COPY);
}

void GTSaver::setAttribute(GHXMLNode& node, const char* attrName, GHString::CharHandlingType nameHandling, const GHPoint2& point) const
{
	const int BUFSZ = 256;
	char buffer[BUFSZ];
	snprintf(buffer, BUFSIZ, "%f %f", point[0], point[1]);
	node.setAttribute(attrName, nameHandling, buffer, GHString::CHT_COPY);
}

void GTSaver::setAttribute(GHXMLNode& ret, const char* attrName, GHString::CharHandlingType nameHandling, GHAlign::Enum align) const
{
	ret.setAttribute(attrName, nameHandling, align == GHAlign::A_RIGHT ? "A_RIGHT" : (align == GHAlign::A_LEFT ? "A_LEFT" : "A_CENTER"), GHString::CHT_REFERENCE);
}

void GTSaver::setAttribute(GHXMLNode& ret, const char* attrName, GHString::CharHandlingType nameHandling, GHFillType::Enum fillType) const
{
	ret.setAttribute(attrName, nameHandling, fillType == GHFillType::FT_PCT ? "FT_PCT" : "FT_PIXELS", GHString::CHT_REFERENCE);
}

