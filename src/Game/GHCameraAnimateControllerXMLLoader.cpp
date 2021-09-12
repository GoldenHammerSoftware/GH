#include "GHCameraAnimateControllerXMLLoader.h"
#include "GHCameraAnimateController.h"
#include "GHXMLNode.h"
#include "GHUtils/GHPropertyContainer.h"
#include "GHXMLObjFactory.h"

GHCameraAnimateControllerXMLLoader::GHCameraAnimateControllerXMLLoader(const GHXMLObjFactory& xmlObjFactory,
                                                                       const GHStringIdFactory& idFactory,
                                                                       const GHTimeVal& timeVal,
                                                                       GHCamera& defaultTargetCamera)
    : mXMLObjFactory(xmlObjFactory)
    , mIdFactory(idFactory)
    , mTimeVal(timeVal)
    , mDefaultTargetCamera(defaultTargetCamera)
{

}

void* GHCameraAnimateControllerXMLLoader::create(const GHXMLNode& node, GHPropertyContainer& extraData) const
{
    GHCamera* targetCam = nullptr;
    GHIdentifier camProp;
    if (node.readAttrIdentifier("camProp", camProp, mIdFactory))
    {
        targetCam = (GHCamera*)extraData.getProperty(camProp);
    }

    if (!targetCam)
    {
        targetCam = &mDefaultTargetCamera;
    }

    size_t numExpectedWaypoints = node.getChildren().size();
    GHCameraAnimateController* ret = new GHCameraAnimateController(mTimeVal, *targetCam, numExpectedWaypoints);
    populate(ret, node, extraData);
    return ret;
}

void GHCameraAnimateControllerXMLLoader::populate(void* obj, const GHXMLNode& node, GHPropertyContainer& extraData) const
{
    GHCameraAnimateController* ret = (GHCameraAnimateController*)obj;
    if (!ret) {
        return;
    }

    float startTime = 0;
    if (node.readAttrFloat("startTime", startTime)) {
        ret->setAnimationStartTime(startTime);
    }

    const GHXMLNode* onEndTransitionNode = node.getChild("onEndTransition", false);
    if (onEndTransitionNode && onEndTransitionNode->getChildren().size())
    {
        GHTransition* onEndTransition = (GHTransition*)mXMLObjFactory.load(*(onEndTransitionNode->getChildren()[0]), &extraData);
        if (onEndTransition)
        {
            ret->setOnEndTransition(onEndTransition);
        }
    }

    const GHXMLNode* waypointsNode = node.getChild("waypoints", false);
    if (!waypointsNode) {
        return;
    }

    GHCamera camera;
    for (const GHXMLNode* child : waypointsNode->getChildren())
    {
        child->readAttrFloatArr("transform", camera.getTransform().getMatrix().getArr(), 16);
        float timeSincePrev = 0;
        child->readAttrFloat("timeSincePrev", timeSincePrev);
        if (timeSincePrev < 0) {
            timeSincePrev = 0;
        }

        bool onlyAnimateTransform = true; //So far this is all we support
        ret->addWaypoint(camera, timeSincePrev, onlyAnimateTransform);
    }
}