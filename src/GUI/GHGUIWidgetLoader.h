// Copyright Golden Hammer Software
#pragma once

#include "GHMath/GHPoint.h"

class GHStateMachineXMLLoader;
class GHPropertyContainer;
class GHXMLNode;
class GHGUIWidget;
class GHXMLObjFactory;
class GHStringIdFactory;

/*
 <blah>
    <widgetStates pixelSizeMod=1>
        // state machine with the states from GHWidgetStates.
    </widgetStates>
 </blah>
*/
// widget class for loading things common to all widgets.
class GHGUIWidgetLoader
{
public:
    GHGUIWidgetLoader(const GHStateMachineXMLLoader& stateMachineLoader,
                      const GHXMLObjFactory& xmlFactory,
                      const GHStringIdFactory& hashTable);
    
    void populate(const GHXMLNode& node, GHGUIWidget& ret,
                  GHPropertyContainer& extraData) const;
    
private:
    // recurse through the tree and convert all children to using FT_PCT
    void convertToPct(GHGUIWidget& widget, const GHPoint<float, 2>& parentSize);

private:
    const GHStateMachineXMLLoader& mStateMachineLoader;
    const GHXMLObjFactory& mXMLFactory;
    const GHStringIdFactory& mIdFactory;
};
