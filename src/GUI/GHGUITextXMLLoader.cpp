// Copyright Golden Hammer Software
#include "GHGUITextXMLLoader.h"
#include "GHGUIText.h"
#include "GHXMLNode.h"
#include "GHUtils/GHResourceFactory.h"
#include "GHFillType.h"
#include "GHGUIWidgetLoader.h"
#include "GHUtils/GHPropertyStacker.h"
#include "GHGUIProperties.h"

GHGUITextXMLLoader::GHGUITextXMLLoader(GHResourceFactory& resourceCache,
                                       const GHIdentifierMap<int>& enumMap,
                                       const GHFontRenderer& fontRenderer,
                                       GHGUIRenderable& renderer,
                                       const GHGUIRectMaker& rectMaker,
                                       const GHGUIWidgetLoader& widgetLoader)
: mFontRenderer(fontRenderer)
, mEnumMap(enumMap)
, mGUIRenderable(renderer)
, mRectMaker(rectMaker)
, mResourceCache(resourceCache)
, mWidgetLoader(widgetLoader)
{
}

void* GHGUITextXMLLoader::create(const GHXMLNode& node, GHPropertyContainer& extraData) const
{
    const char* text = node.getAttribute("text");
    const char* fontName = node.getAttribute("font");
    if (!fontName) return 0;
    GHFont* font = (GHFont*)mResourceCache.getCacheResource(fontName);
    if (!font) return 0;
    
    float height = 15;
    node.readAttrFloat("textHeight", height);
    GHFillType::Enum fillType = GHFillType::FT_PIXELS;
    node.readAttrEnum("fill", (int&)fillType, mEnumMap);
    GHAlign::Enum hAlign = GHAlign::A_LEFT;
    node.readAttrEnum("hAlign", (int&)hAlign, mEnumMap);
    GHAlign::Enum vAlign = GHAlign::A_TOP;
    node.readAttrEnum("vAlign", (int&)vAlign, mEnumMap);
    float drawLayer = 0;
    node.readAttrFloat("drawLayer", drawLayer);
	bool wrap = false;
	node.readAttrBool("wrap", wrap);

    GHGUIText* guiText = new GHGUIText(mFontRenderer,
                                       mGUIRenderable,
                                       mRectMaker,
                                       font, 
                                       text, height, wrap, fillType,
                                       hAlign, vAlign,
                                       drawLayer);

	GHGUIWidgetResource* ret = new GHGUIWidgetResource(guiText);

	GHPropertyStacker parentStack(extraData, GHGUIProperties::GP_PARENTWIDGET, GHProperty(guiText));
	GHPropertyStacker parentResStack(extraData, GHGUIProperties::GP_PARENTWIDGETRES, GHProperty(ret));
    mWidgetLoader.populate(node, *guiText, extraData);

    return ret;
}


