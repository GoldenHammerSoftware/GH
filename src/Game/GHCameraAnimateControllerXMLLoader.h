#pragma once

#include "GHXMLObjLoader.h"

class GHXMLObjFactory;
class GHStringIdFactory;
class GHTimeVal;
class GHCamera;
class GHStateMachine;

//<camAnim camProp="notDefaultCamera"> //camProp optional. Will look on extraData for a GHCamera*
//	<waypoints>
//		<wp 
//          transform="1 0 0 0 0 1 0 0 0 0 1 0 0 0 0 1" //Transform is mandatory (at least for now)
//          timeSincePrev=0   //Optional-ish. Will default to 0, which will skip the previous waypoint. Negative numbers will also be set to 0.
//      />               	
//		//Todo: add XML loading for other camera properties if we need them.
//	</waypoints>
//  <onEndTransition> 
//      //Completely optional. If exists, activate a transition after reaching the last waypoint.
//  </onEndTransition>
//</camAnim>
class GHCameraAnimateControllerXMLLoader : public GHXMLObjLoader
{
public:
    GHCameraAnimateControllerXMLLoader(const GHXMLObjFactory& xmlObjFactory,
                                       const GHStringIdFactory& idFactory,
                                       const GHTimeVal& timeVal,
                                       GHCamera& defaultTargetCamera);

    virtual void* create(const GHXMLNode& node, GHPropertyContainer& extraData) const;
    virtual void populate(void* obj, const GHXMLNode& node, GHPropertyContainer& extraData) const;

private:
    const GHXMLObjFactory& mXMLObjFactory;
    const GHStringIdFactory& mIdFactory;
    const GHTimeVal& mTimeVal;
    GHCamera& mDefaultTargetCamera;
};
