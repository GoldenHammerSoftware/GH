// Copyright Golden Hammer Software
#pragma once

#include "GHXMLObjLoader.h"

class GHPropertyContainer;
class GHStringIdFactory;
class GHControllerMgr;
class GHInputState;
class GHInputClaim;

/*
 <guiPanel>
    <guiPos xFill=FT_PIXELS yFill=FT_PIXELS align="0.5 0.5" offset="0 0" size="100 10"/>
    <renderable>
        <guiQuadRenderable>
            <ghm vertex=guivertex.glsl pixel=testpixel.glsl zread="true" zwrite=false/>
        </guiQuadRenderable>
    </renderable>
    <children>
        <guiPanel>
            <guiPos xFill=FT_PIXELS yFill=FT_PIXELS align="0 0.5" offset="0 0" size="10 10"/>
            <renderable>
                <guiQuadRenderable>
                    <ghm vertex=guivertex.glsl pixel=testpixel.glsl zread="true" zwrite=false/>
                </guiQuadRenderable>
            </renderable>
            <widgetStates>
                <transitions stateEnum="WS_ACTIVE">
                    <guiSlider prop=GP_VOLUME range="0 1" minPos="0 0.5" maxPos="1 0.5">
                </transitions>
            </widgetStates>
        </guiPanel>
    </children>
 </guiPanel>
*/
// Used to load a controller that will move a knob around on a slider setting property values.
class GHGUISliderXMLLoader : public GHXMLObjLoader
{
public:
    GHGUISliderXMLLoader(GHPropertyContainer& props, const GHStringIdFactory& hashTable,
                         GHControllerMgr& controllerManager, GHInputState& inputState,
						 GHInputClaim& inputClaim);

    virtual void* create(const GHXMLNode& node, GHPropertyContainer& extraData) const;
    GH_NO_POPULATE
    
private:
    GHPropertyContainer& mProps;
    const GHStringIdFactory& mIdFactory;
    GHControllerMgr& mControllerManager;
    GHInputState& mInputState;
	GHInputClaim& mInputClaim;
};
