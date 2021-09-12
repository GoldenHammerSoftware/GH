// Copyright Golden Hammer Software
#pragma once

#include "GHXMLObjLoader.h"
#include "GHGUIRectMaker.h"

class GHGUIRenderable;
class GHScreenInfo;
class GHXMLObjFactory;
class GHGUIPanel;
class GHGUIWidgetLoader;

/* format:
 <guiPanel>
    <guiPos xFill=pixels yFill=pixels align="0 0" offset="0 0" size="50 50"/>
    <renderable>
        <guiQuadRenderable>
            <ghm vertex=guivertex.glsl pixel=testpixel.glsl zread="true" zwrite=false/>
        </guiQuadRenderable>
    </renderable>
    <children>
        <guiText text="^900blah" font=ericfont.xml textHeight=25 fill=FT_PIXELS/>
    </children>
 </guiPanel>
*/
class GHGUIPanelXMLLoader : public GHXMLObjLoader
{
public:
    GHGUIPanelXMLLoader(const GHXMLObjFactory& xmlFactory,
                        GHGUIRenderable& renderer, 
                        const GHGUIRectMaker& rectMaker,
                        const GHGUIWidgetLoader& widgetLoader);
    
    virtual void* create(const GHXMLNode& node, GHPropertyContainer& extraData) const;
    virtual void populate(void* obj, const GHXMLNode& node, GHPropertyContainer& extraData) const;

private:
    const GHXMLObjFactory& mXMLFactory;
    GHGUIRenderable& mRenderer;
    const GHGUIRectMaker& mRectMaker;
    const GHGUIWidgetLoader& mWidgetLoader;
};
