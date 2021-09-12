// Copyright Golden Hammer Software
#include "GHGUIWidgetLoader.h"
#include "GHStateMachineXMLLoader.h"
#include "GHXMLNode.h"
#include "GHGUIWidget.h"
#include "GHXMLObjFactory.h"
#include "GHPlatform/GHDebugMessage.h"
#include "GHPropertyContainerXMLLoader.h"
#include "GHGUIProperties.h"

GHGUIWidgetLoader::GHGUIWidgetLoader(const GHStateMachineXMLLoader& stateMachineLoader,
                                     const GHXMLObjFactory& xmlFactory,
                                     const GHStringIdFactory& hashTable)
: mStateMachineLoader(stateMachineLoader)
, mXMLFactory(xmlFactory)
, mIdFactory(hashTable)
{
}

void GHGUIWidgetLoader::populate(const GHXMLNode& node, GHGUIWidget& ret,
    GHPropertyContainer& extraData) const
{
    float pixelSizeMod = 1.0f;
    node.readAttrFloat("pixelSizeMod", pixelSizeMod);
    ret.setPixelSizeMod(pixelSizeMod);

    bool debugging = false;
    if (node.readAttrBool("debugging", debugging))
    {
        ret.debugging = debugging;
    }
    
    const GHXMLNode* smNode = node.getChild("widgetStates", false);
    if (smNode)
    {
        mStateMachineLoader.populate(&ret.getWidgetStates(), *smNode, extraData);
    }
    
    const GHXMLNode* posNode = node.getChild("guiPos", false);
    if (posNode) {
        GHGUIPosDesc posDesc = ret.getPosDesc();
        const GHXMLObjLoader* descLoader = mXMLFactory.getLoader("guiPos");
        if (!descLoader) {
            GHDebugMessage::outputString("Failed to find guiPos loader.");
        }
        else
        {
            descLoader->populate(&posDesc, *posNode, extraData);
            ret.setPosDesc(posDesc);
        }
    }
    
    const char* idString = node.getAttribute("id");
    if (idString)
    {
        ret.setId(mIdFactory.generateHash(idString));
    }

    const GHXMLNode* propsNode = node.getChild("properties", false);
    if (propsNode)
    {
        GHPropertyContainerXMLLoader* propLoader = (GHPropertyContainerXMLLoader*)mXMLFactory.getLoader("properties");
        if (!propLoader) {
            GHDebugMessage::outputString("Could not find properties loader for gui widget");
        }
        else {
            propLoader->populate(&ret.getPropertyContainer(), *propsNode, extraData);
        }
    }

    //Can't use GHPropertyStacker here since we aren't guaranteed it's a valid property and we don't want to overwrite parent canvases unless it is
    const GHProperty& canvasProp = ret.getPropertyContainer().getProperty(GHGUIProperties::GP_GUICANVAS);
    GHProperty oldCanvasProp;
    if (canvasProp.isValid())
    {
        oldCanvasProp = extraData.getProperty(GHGUIProperties::GP_GUICANVAS);
        extraData.setProperty(GHGUIProperties::GP_GUICANVAS, canvasProp);
    }

    const GHXMLNode* childrenNode = node.getChild("children", false);
    if (childrenNode)
    {
        for (size_t i = 0; i < childrenNode->getChildren().size(); ++i)
        {
            const GHXMLNode* childNode = childrenNode->getChildren()[i];
            GHGUIWidgetResource* childWidget = (GHGUIWidgetResource*)mXMLFactory.load(*childNode, &extraData);
            if (childWidget)
            {
                ret.addChild(childWidget);
            }
        }
    }

    if (canvasProp.isValid())
    {
        extraData.setProperty(GHGUIProperties::GP_GUICANVAS, oldCanvasProp);
    }
}

void GHGUIWidgetLoader::convertToPct(GHGUIWidget& widget, const GHPoint<float, 2>& parentSize)
{
    
}
