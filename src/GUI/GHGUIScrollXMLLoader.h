// Copyright Golden Hammer Software
#pragma once

#include "GHXMLObjLoader.h"

class GHGUIPanelXMLLoader;
class GHGUIPosDescXMLLoader;
class GHControllerMgr;
class GHGUIPosDesc;
class GHInputClaim;
class GHInputState;
class GHScreenInfo;
class GHTimeVal;
class GHGUIWidget;
class GHGUIScroll;
class GHGUIRectMaker;

//Loads a GHGUIPanel that scrolls its children up and down
// based on user input.
//Format:
//<guiScroll 
//        triggerThreshold="0"      //<-- optional: The amount that a button must be depressed in order to be considered for scrolling the gui
//        pointerClaimThreshold="0" //<-- optional: the threshold that the scroll must have moved before claiming the ponter over its children, to distinguish scrolling from clicking on a child button. Default 0
//        momentumDecayFactor="10"> //<-- optional: the factor by which the scroll panel movement will decay after letting go. Default 10.  
//  <!-- the node named "childPos" is the GHGUIPos that all children will be forced to have  -->
//  <childPos xFill=FT_PIXELS yFill=FT_PIXELS align="0.5 0" size="300 67.5" xSizeAlign=A_CENTER ySizeAlign=A_BOTTOM/>
//  <children>
//      <!-- Put children here as normal. Dynamically-added children -->
//      <!--  will also scroll with the ones added here. -->
//  </children>
//
//  <!-- Optionally, fill as normal for a GUIPanel, background, etc.-->
//
// </guiScroll>
class GHGUIScrollXMLLoader : public GHXMLObjLoader
{
public:
    GHGUIScrollXMLLoader(const GHGUIPanelXMLLoader& panelLoader,
                         const GHGUIPosDescXMLLoader& posDescLoader,
                         GHControllerMgr& controllerMgr,
                         GHInputState& inputState,
                         GHInputClaim& inputClaim,
                         const GHScreenInfo& screenInfo,
                         const GHTimeVal& timeVal,
						 const GHGUIRectMaker& rectMaker);
    
    virtual void* create(const GHXMLNode& node, GHPropertyContainer& extraData) const;
	// children of inherit panels will not have access to the GHGUIScroll.
    virtual void populate(void* obj, const GHXMLNode& node, GHPropertyContainer& extraData) const;
    
private:
    void populateScrollParams(void* obj, const GHXMLNode& node, GHPropertyContainer& extraData) const;
    void loadChildPosDesc(GHGUIPosDesc& posDesc, const GHXMLNode& parentNode, GHPropertyContainer& extraData) const;

private:
    const GHGUIPanelXMLLoader& mPanelLoader;
    const GHGUIPosDescXMLLoader& mPosDescLoader;
    GHControllerMgr& mControllerMgr;
    GHInputState& mInputState;
    GHInputClaim& mInputClaim;
    const GHScreenInfo& mScreenInfo;
    const GHTimeVal& mTimeVal;
	const GHGUIRectMaker& mRectMaker;
};
