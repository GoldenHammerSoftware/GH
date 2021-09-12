// Copyright 2010 Golden Hammer Software
#include "GTUtil.h"
#include "GHXMLObjFactory.h"
#include "GHXMLNode.h"
#include "GHGUIWidgetRenderer.h"
#include "GHGUIPanel.h"
#include "GHMath/GHRandom.h"
#include "GTMetadataList.h"
#include "GTChangePusher.h"
#include "GHGUIText.h"

GTUtil::GTUtil(const GHXMLObjFactory& xmlObjFactory, const GHStringIdFactory& idFactory, GHGUIWidgetResource& topGUI, GTMetadataList& metadataList)
	: mXMLObjFactory(xmlObjFactory)
	, mIdFactory(idFactory)
	, mTopPanel(topGUI)
	, mMetadataList(metadataList)
	, mIDForAllTexts(idFactory.generateHash("idForAllTexts")) 
{

}

GHGUIWidgetResource* GTUtil::findParent(GHGUIWidgetResource& potentialParent, const GHGUIWidgetResource& target)
{
	size_t numChildren = potentialParent.get()->getNumChildren();
	for (size_t i = 0; i < numChildren; ++i)
	{
		GHGUIWidgetResource* child = potentialParent.get()->getChildAtIndex(i);
		if (child == &target)
		{
			return &potentialParent;
		}

		GHGUIWidgetResource* nextPotentialParent = findParent(*child, target);
		if (nextPotentialParent) {
			return nextPotentialParent;
		}
	}
	return 0;
}

void GTUtil::setRandomColor(GHGUIWidgetResource& widget)
{
	GHRandom random; random;
	GHPoint4 randomColor(random.getRandFloat(1.f), random.getRandFloat(1.f), random.getRandFloat(1.f), .75f);
	setColor(widget, randomColor, false);
}

void GTUtil::setColor(GHGUIWidgetResource& widget, const GHPoint4& color, GTChangePusher* changePusher)
{
	GHXMLNode* matNode = createColorMatNode(color);
	setMaterial(widget, matNode, changePusher);
}

void GTUtil::setTexture(GHGUIWidgetResource& widget, const char* filename, GTChangePusher& changePusher)
{
	GHXMLNode* matNode = createTextureMatNode(filename, GHString::CHT_REFERENCE);
	setMaterial(widget, matNode, &changePusher);
}

void GTUtil::setMaterial(GHGUIWidgetResource& widget, GHXMLNode* matNode, GTChangePusher* changePusher)
{
	GHXMLNode renderableNode;
	renderableNode.setName("guiQuadRenderable", GHString::CHT_REFERENCE);
	renderableNode.addChild(matNode);

	setRenderableNode(widget, renderableNode, changePusher);
}

void GTUtil::setRenderableNode(GHGUIWidgetResource& widget, const GHXMLNode& renderableNode, GTChangePusher* changePusher)
{
	GHGUIWidgetRenderer* renderer = (GHGUIWidgetRenderer*)mXMLObjFactory.load(renderableNode);
	if (renderer) {

		GHGUIWidgetResource* parent = findParent(mTopPanel, widget);
		if (parent) {
			widget.acquire();
			parent->get()->removeChild(&widget);
		}

		((GHGUIPanel*)widget.get())->setWidgetRenderer(renderer);
		renderer->setWidget(widget.get());

		if (parent) {
			parent->get()->addChild(&widget);
			widget.release();
		}

		if (changePusher) {
			GTMetadata* metadata = changePusher->createMetadataClone();
			delete metadata->mRenderableNode;
			metadata->mRenderableNode = renderableNode.clone();
			changePusher->pushChange(metadata);
		}
	}
}

void fillTextParameters(const GHGUIText* text, GTMetadata::TextParameters& outParams)
{
	if (!text)
	{
		outParams = GTMetadata::TextParameters();
		return;
	}

	outParams.mHeight = text->getTextHeight();
	outParams.mFill = text->getHeightFillType();
	outParams.mHAlign = text->getHAlign();
	outParams.mVAlign = text->getVAlign();
	outParams.mWrap = text->getWrap();
}

bool GTUtil::getTextParameters(const GHGUIWidgetResource& widget, GTMetadata::TextParameters& outTextParameters, const char** outText)
{
	const GHGUIWidgetResource* textChild = getTextChild(widget);

	if (!textChild)
	{
		return false;
	}

	GHGUIText* guiText = (GHGUIText*)textChild->get();
	fillTextParameters(guiText, outTextParameters);

	if (outText)
	{
		*outText = guiText->getText();
	}

	return true;
}

void GTUtil::setText(GHGUIWidgetResource& widget, const char* text, GTChangePusher* changePusher)
{
	GHGUIWidgetResource* textChild = getTextChild(widget);

	GHGUIText* guiText = 0;

	//if empty text, clear
	if (!text || !*text)
	{
		if (textChild)
		{
			widget.get()->removeChild(textChild);
		}
	}
	else
	{
		if (!textChild)
		{
			textChild = createText();
			textChild->get()->setId(mIDForAllTexts);
			widget.get()->addChild(textChild);
		}

		//this is expected to be a GHGUIText
		guiText = (GHGUIText*)textChild->get();
		guiText->setText(text);
	}

	if (changePusher)
	{
		GTMetadata* metadata = changePusher->createMetadataClone();
		metadata->mText.setConstChars(text, GHString::CHT_COPY);
		fillTextParameters(guiText, metadata->mTextParameters);
		changePusher->pushChange(metadata);
	}
}

GHGUIWidgetResource* GTUtil::getTextChild(GHGUIWidgetResource& widget) const
{
	return const_cast<GHGUIWidgetResource*>(getTextChild(const_cast<const GHGUIWidgetResource&>(widget)));
}

//getChild is recursive. We explicitly don't want to be recursive here -- we only want the direct text child of the panel if it exists
const GHGUIWidgetResource* GTUtil::getTextChild(const GHGUIWidgetResource& widget) const
{
	size_t numChildren = widget.get()->getNumChildren();
	for(size_t i = 0; i < numChildren; ++i)
	{ 
		const GHGUIWidgetResource* child = widget.get()->getChildAtIndex(i);
		if (child->get()->getId() == mIDForAllTexts)
		{
			return child;
		}
	}
	return 0;
}

void GTUtil::setTextParameters(GHGUIWidgetResource& widget, const GTMetadata::TextParameters& textParameters, GTChangePusher* changePusher)
{
	GHGUIWidgetResource* textChild = getTextChild(widget);

	if (!textChild)
	{
		return;
	}


	//this is expected to be a GHGUIText
	GHGUIText* guiText = (GHGUIText*)textChild->get();
	guiText->setTextHeight(textParameters.mHeight);
	guiText->setHeightFillType(textParameters.mFill);
	guiText->setAlignment(textParameters.mHAlign, textParameters.mVAlign);
	guiText->setWrap(textParameters.mWrap);

	if (changePusher)
	{
		GTMetadata* metadata = changePusher->createMetadataClone();
		metadata->mTextParameters = textParameters;
		changePusher->pushChange(metadata);
	}
}

bool GTUtil::setUVAngle(GHGUIWidgetResource& widget, float uvAngle, GTChangePusher& changePusher)
{
	GTMetadata* metadata = mMetadataList.getNode(&widget);
	if (!metadata || !metadata->mRenderableNode)
	{
		return false;
	}
	
	GHXMLNode* renderableNode = metadata->mRenderableNode->clone();
	
	char buf[50];
	snprintf(buf, 50, "%f", uvAngle);
	renderableNode->setAttribute("uvAngle", GHString::CHT_REFERENCE, buf, GHString::CHT_COPY);

	setRenderableNode(widget, *renderableNode, &changePusher);

	delete renderableNode;

	return true;
}

GHGUIWidgetResource* GTUtil::createPanel(void)
{
	GHXMLNode* panelNode = createEmptyPanelNode();
	GHGUIWidgetResource* panel = (GHGUIWidgetResource*)mXMLObjFactory.load(*panelNode);
	delete panelNode;
	return panel;
}

//creates a GUIText for inserting as a child to a GuiPanel that
// will represent it in the tool.
GHGUIWidgetResource* GTUtil::createText(void)
{
	GHXMLNode* panelNode = createEmptyPanelNode();
	panelNode->setName("guiText", GHString::CHT_REFERENCE);
	panelNode->setAttribute("text", GHString::CHT_REFERENCE, "ExampleText", GHString::CHT_REFERENCE);
	panelNode->setAttribute("font", GHString::CHT_REFERENCE, "ericfont.xml", GHString::CHT_REFERENCE);
	panelNode->setAttribute("textHeight", GHString::CHT_REFERENCE, "0.5", GHString::CHT_REFERENCE);
	panelNode->setAttribute("fill", GHString::CHT_REFERENCE, "FT_PCT", GHString::CHT_REFERENCE);
	panelNode->setAttribute("hAlign", GHString::CHT_REFERENCE, "A_CENTER", GHString::CHT_REFERENCE);
	panelNode->setAttribute("vAlign", GHString::CHT_REFERENCE, "A_CENTER", GHString::CHT_REFERENCE);

	GHGUIWidgetResource* panel = (GHGUIWidgetResource*)mXMLObjFactory.load(*panelNode);
	delete panelNode;
	return panel;
}

//local utility: always uses CHT_REFERENCE
inline void copyAttributeByReference(const GHXMLNode& srcNode, GHXMLNode& dstNode, const char* attrName)
{
	const char* attribute = srcNode.getAttribute(attrName);
	if (attribute)
	{
		dstNode.setAttribute(attrName, GHString::CHT_REFERENCE, attribute, GHString::CHT_REFERENCE);
	}
}

//creates a GUIText for inserting as a child to a GuiPanel that
// will represent it in the tool. Adopts text-specific parameters from the input node.
//Not just passing node in directly because we want to maintain some consistency
// with our convention of in the tool forcing all the position description to be on a gui panel
// that is the parent of the guitext. This means forcing the child text node to always be 0-1 wrt its parent (the guipanel representing it)
// (We have this convention to circumvent not having built-in reflection in the gui system).
//If we pass the node in directly then we will have inconsistency with the way we save and load files,
// where the parent panel becomes a guiText on save and keeps its position description.
//eg:
// in GHGUITool: <guiPanel>   //this is the panel representing the text
//					<posDesc ... />
//					<children> <guiText id="idForAllTexts"> </children> //This is how we implement reflection of guiTexts in the tool
//				 </guiPanel
//
// on save this becomes <guiText>
//							<posDesc .../>
//						</guiText>
// so on load we need to undo that by loading the posdesc as part 
// of the parent panel and the text parameters as part of the text node
//Thus we factor out the pos desc here.
GHGUIWidgetResource* GTUtil::createSanitizedTextChild(const GHXMLNode& node)
{
	GHXMLNode* panelNode = createEmptyPanelNode();
	panelNode->setName("guiText", GHString::CHT_REFERENCE);
	copyAttributeByReference(node, *panelNode, "text");
	copyAttributeByReference(node, *panelNode, "font");
	copyAttributeByReference(node, *panelNode, "textHeight");
	copyAttributeByReference(node, *panelNode, "fill");
	copyAttributeByReference(node, *panelNode, "hAlign");
	copyAttributeByReference(node, *panelNode, "vAlign");

	GHGUIWidgetResource* panel = (GHGUIWidgetResource*)mXMLObjFactory.load(*panelNode);
	delete panelNode;
	return panel;

}

GHXMLNode* GTUtil::createEmptyPanelNode(void)
{
	GHXMLNode* panelNode = new GHXMLNode;
	panelNode->setName("guiPanel", GHString::CHT_REFERENCE);
	char idBuf[256];
	static int count = 0;
	snprintf(idBuf, 256, "Panel %d", count++);
	panelNode->setAttribute("id", GHString::CHT_REFERENCE, idBuf, GHString::CHT_COPY);
	return panelNode;
}

GHXMLNode* GTUtil::createColorMatNode(const GHPoint4 &color)
{
	GHXMLNode* matNode = new GHXMLNode;
	matNode->setName("ghm", GHString::CHT_REFERENCE);
	matNode->setAttribute("vertex", GHString::CHT_REFERENCE, "guivertex.hlsl", GHString::CHT_REFERENCE);
	matNode->setAttribute("pixel", GHString::CHT_REFERENCE, "colorpixel.glsl", GHString::CHT_REFERENCE);
	matNode->setAttribute("zread", GHString::CHT_REFERENCE, "false", GHString::CHT_REFERENCE);
	matNode->setAttribute("zwrite", GHString::CHT_REFERENCE, "false", GHString::CHT_REFERENCE);
	matNode->setAttribute("billboard", GHString::CHT_REFERENCE, "none", GHString::CHT_REFERENCE);
	matNode->setAttribute("draworder", GHString::CHT_REFERENCE, "0", GHString::CHT_REFERENCE);
	matNode->setAttribute("cullmode", GHString::CHT_REFERENCE, "none", GHString::CHT_REFERENCE);
	matNode->setAttribute("alphablend", GHString::CHT_REFERENCE, "true", GHString::CHT_REFERENCE);
	matNode->setAttribute("srcblend", GHString::CHT_REFERENCE, "alpha", GHString::CHT_REFERENCE);
	matNode->setAttribute("dstblend", GHString::CHT_REFERENCE, "invalpha", GHString::CHT_REFERENCE);

	GHXMLNode* floatArgsNode = new GHXMLNode;
	matNode->addChild(floatArgsNode);
	floatArgsNode->setName("floatargs", GHString::CHT_REFERENCE);

	char colorStr[256];
	snprintf(colorStr, 256, "%f %f %f %f", color[0], color[1], color[2], color[3]);

	GHXMLNode* colorNode = new GHXMLNode;
	floatArgsNode->addChild(colorNode);
	colorNode->setName("floatarg", GHString::CHT_REFERENCE);
	colorNode->setAttribute("handle", GHString::CHT_REFERENCE, "TintColor", GHString::CHT_REFERENCE);
	colorNode->setAttribute("count", GHString::CHT_REFERENCE, "4", GHString::CHT_REFERENCE);
	colorNode->setAttribute("value", GHString::CHT_REFERENCE, colorStr, GHString::CHT_COPY);

	return matNode;
}

GHXMLNode* GTUtil::createTextureMatNode(const char* texFile, GHString::CharHandlingType texFileHandlingType)
{
	GHXMLNode* matNode = new GHXMLNode;
	matNode->setName("ghm", GHString::CHT_REFERENCE);
	matNode->setAttribute("vertex", GHString::CHT_REFERENCE, "guivertex.hlsl", GHString::CHT_REFERENCE);
	matNode->setAttribute("pixel", GHString::CHT_REFERENCE, "pixel-1tex.glsl", GHString::CHT_REFERENCE);
	matNode->setAttribute("zread", GHString::CHT_REFERENCE, "false", GHString::CHT_REFERENCE);
	matNode->setAttribute("zwrite", GHString::CHT_REFERENCE, "false", GHString::CHT_REFERENCE);
	matNode->setAttribute("billboard", GHString::CHT_REFERENCE, "none", GHString::CHT_REFERENCE);
	matNode->setAttribute("draworder", GHString::CHT_REFERENCE, "0", GHString::CHT_REFERENCE);
	matNode->setAttribute("cullmode", GHString::CHT_REFERENCE, "none", GHString::CHT_REFERENCE);
	matNode->setAttribute("alphablend", GHString::CHT_REFERENCE, "true", GHString::CHT_REFERENCE);
	matNode->setAttribute("srcblend", GHString::CHT_REFERENCE, "alpha", GHString::CHT_REFERENCE);
	matNode->setAttribute("dstblend", GHString::CHT_REFERENCE, "invalpha", GHString::CHT_REFERENCE);

	GHXMLNode* texListNode = new GHXMLNode;
	texListNode->setName("textures", GHString::CHT_REFERENCE);
	matNode->addChild(texListNode);

	GHXMLNode* textureNode = new GHXMLNode;
	texListNode->addChild(textureNode);

	textureNode->setName("texture", GHString::CHT_REFERENCE);
	textureNode->setAttribute("file", GHString::CHT_REFERENCE, texFile, texFileHandlingType);
	textureNode->setAttribute("handle", GHString::CHT_REFERENCE, "Texture0", GHString::CHT_REFERENCE);
	textureNode->setAttribute("wrap", GHString::CHT_REFERENCE, "wrap", GHString::CHT_REFERENCE);
	textureNode->setAttribute("mipmap", GHString::CHT_REFERENCE, "false", GHString::CHT_REFERENCE);

	return matNode;
}

GHGUIWidgetResource* GTUtil::createPanel(const GHXMLNode& node, const GHIdentifier& parentId)
{
	GHXMLNode* panelNode = createEmptyPanelNode();

	const GHXMLNode* guiPosNode = (node.getChild("guiPos", false));
	if (guiPosNode) {
		GHXMLNode* clonedPosNode = guiPosNode->clone();
		panelNode->addChild(clonedPosNode);
	}

	GHGUIWidgetResource* panel = (GHGUIWidgetResource*)mXMLObjFactory.load(*panelNode);

	if (!strcmp(node.getName(), "guiText"))
	{
		GHGUIWidgetResource* textResource = createSanitizedTextChild(node);
		textResource->get()->setId(mIDForAllTexts);
		panel->get()->addChild(textResource);
	}

	const char* id = node.getAttribute("id");
	if (id)
	{
		panel->get()->setId(mIdFactory.generateHash(id));
	}

	GHXMLNode* savedRenderableNode = 0;

	const GHXMLNode* renderableParentNode = node.getChild("renderable", false);
	if (renderableParentNode
		&& renderableParentNode->getChildren().size())
	{
		GHXMLNode* renderableNode = renderableParentNode->getChildren()[0]->clone();
		GHGUIWidgetRenderer* widgetRenderer = (GHGUIWidgetRenderer*)mXMLObjFactory.load(*renderableNode);
		if (widgetRenderer)
		{
			((GHGUIPanel*)panel->get())->setWidgetRenderer(widgetRenderer);
			widgetRenderer->setWidget(panel->get());
			savedRenderableNode = renderableNode;
		}
	}
	else {
		setRandomColor(*panel);
	}

	GTMetadata* metadata = new GTMetadata(panel->get()->getId(), parentId, panel->get()->getPosDesc(), savedRenderableNode, 0);
	mMetadataList.setNode(panel, metadata);

	const GHXMLNode* childrenNode = node.getChild("children", false);
	if (childrenNode)
	{
		size_t numChildren = childrenNode->getChildren().size();
		for (size_t i = 0; i < numChildren; ++i)
		{
			const GHXMLNode* childNode = childrenNode->getChildren()[i];
			GHGUIWidgetResource* child = createPanel(*childNode, panel->get()->getId());
			panel->get()->addChild(child);
		}
	}

	delete panelNode;
	return panel;
}
