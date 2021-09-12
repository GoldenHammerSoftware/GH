// Copyright Golden Hammer Software
#pragma once

#include "GHXMLObjLoader.h"
#include "GHString/GHIdentifierMap.h"

class GHFontRenderer;
class GHGUIRenderable;
class GHGUIRectMaker;
class GHResourceFactory;
class GHGUIWidgetLoader;

/*
 // ^900 means full red color, it is not actually displayed.
 // we do a sprintf of the text string with the properties in order as the arguments.
<guiText text="^900blah blah %s" font=ericfont.xml textHeight=25 fill=FT_PIXELS hAlign=A_LEFT vAlign=A_TOP>
 <properties>
    <property name="fps"/>
 </properties>
</guiText>
*/
class GHGUITextXMLLoader : public GHXMLObjLoader
{
public:
    GHGUITextXMLLoader(GHResourceFactory& resourceCache,
                       const GHIdentifierMap<int>& enumMap,
                       const GHFontRenderer& fontRenderer,
                       GHGUIRenderable& renderer,
                       const GHGUIRectMaker& rectMaker,
                       const GHGUIWidgetLoader& widgetLoader);
    
    virtual void* create(const GHXMLNode& node, GHPropertyContainer& extraData) const;
    GH_NO_POPULATE
    
private:
    GHResourceFactory& mResourceCache;
    const GHIdentifierMap<int>& mEnumMap;
    const GHFontRenderer& mFontRenderer;
    GHGUIRenderable& mGUIRenderable;
    const GHGUIRectMaker& mRectMaker;
    const GHGUIWidgetLoader& mWidgetLoader;
};

