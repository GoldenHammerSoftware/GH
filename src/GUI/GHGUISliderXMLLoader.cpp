// Copyright Golden Hammer Software
#include "GHGUISliderXMLLoader.h"
#include "GHGUIProperties.h"
#include "GHGUIWidget.h"
#include "GHUtils/GHPropertyContainer.h"
#include "GHPlatform/GHDebugMessage.h"
#include "GHXMLNode.h"
#include "GHGUISlider.h"
#include "GHUtils/GHControllerTransition.h"

GHGUISliderXMLLoader::GHGUISliderXMLLoader(GHPropertyContainer& props, const GHStringIdFactory& hashTable,
                                           GHControllerMgr& controllerManager, GHInputState& inputState,
										   GHInputClaim& inputClaim)
: mProps(props)
, mIdFactory(hashTable)
, mControllerManager(controllerManager)
, mInputState(inputState)
, mInputClaim(inputClaim)
{
}

void* GHGUISliderXMLLoader::create(const GHXMLNode& node, GHPropertyContainer& extraData) const
{
    GHGUIWidget* knob = extraData.getProperty(GHGUIProperties::GP_PARENTWIDGET);
    if (!knob) {
        GHDebugMessage::outputString("Failed to find parent widget for slider.");
        return 0;
    }
    const char* propStr = node.getAttribute("prop");
    if (!propStr) {
        GHDebugMessage::outputString("No prop specified for slider.");
    }
    GHIdentifier propId = mIdFactory.generateHash(propStr);
    GHPoint2 range(0,1);
    node.readAttrFloatArr("range", range.getArr(), 2);
    GHRect<float,2> bounds;
    bounds.mMin.setCoords(0,0);
    bounds.mMax.setCoords(1,0);
    node.readAttrFloatArr("minPos", bounds.mMin.getArr(), 2);
    node.readAttrFloatArr("maxPos", bounds.mMax.getArr(), 2);
    
    std::vector<int> triggerKeys;
    GHGUISlider* slider = new GHGUISlider(mProps, propId, *knob, bounds, range,
                                          mInputState, mInputClaim, triggerKeys);
    GHControllerTransition* ret = new GHControllerTransition(mControllerManager, slider);
    return ret;
}
