// Copyright Golden Hammer Software
#include "GHGUISwitchXMLLoader.h"
#include "GHUtils/GHTransitionSwitch.h"
#include "GHGUIWidgetTransition.h"
#include "GHString/GHStringIdFactory.h"
#include "GHPlatform/GHDebugMessage.h"
#include "GHXMLNode.h"
#include "GHXMLObjFactory.h"
#include "GHUtils/GHPropertyContainer.h"
#include "GHGUIProperties.h"

GHGUISwitchXMLLoader::GHGUISwitchXMLLoader(const GHPropertyContainer& props, const GHStringIdFactory& hashTable,
                                           GHXMLObjFactory& objFactory, GHControllerMgr& controllerMgr)
: mProps(props)
, mIdFactory(hashTable)
, mObjFactory(objFactory)
, mControllerMgr(controllerMgr)
{
}

void* GHGUISwitchXMLLoader::create(const GHXMLNode& node, GHPropertyContainer& extraData) const
{
    const char* propIdStr = node.getAttribute("prop");
    if (!propIdStr) {
        GHDebugMessage::outputString("No prop specified for gui switch.");
        return 0;
    }

    GHIdentifier propId = mIdFactory.generateHash(propIdStr);
    GHTransitionSwitch* ret = new GHTransitionSwitch(mProps, propId, mControllerMgr);
    populate(ret, node, extraData);
    return ret;
}

void GHGUISwitchXMLLoader::populate(void* obj, const GHXMLNode& node, GHPropertyContainer& extraData) const
{
    GHTransitionSwitch* ret = (GHTransitionSwitch*)obj;
    
    GHGUIWidget* parentWidget = extraData.getProperty(GHGUIProperties::GP_PARENTWIDGET);
    if (!parentWidget) {
        GHDebugMessage::outputString("Failed to find parent widget for switch.");
        return;
    }
  
    const GHXMLNode* switchesNode = node.getChild("switches", false);
    if (!switchesNode)
    {
        // we are phasing out the <switches> node. if not present expect that all children are switches.
        switchesNode = &node;
    }
    
    for (size_t i = 0; i < switchesNode->getChildren().size(); ++i)
    {
        const GHXMLNode* switchChild = switchesNode->getChildren()[i];
        
        bool isDefault = false;
        switchChild->readAttrBool("default", isDefault);
        
        const GHXMLNode* propsNode = switchChild->getChild("properties", false);
        if (!isDefault && !propsNode) {
            GHDebugMessage::outputString("switch with no properties node");
            continue;
        }
        const GHXMLNode* widgetsNode = switchChild->getChild("widgets", false);
        const GHXMLNode* transitionsNode = switchChild->getChild("transitions", false);
        if (!widgetsNode && !transitionsNode) {
            GHDebugMessage::outputString("switch with no widgets or transitions specified");
            continue;
        }
        
        std::vector<GHProperty> propVals;
        
        if (!isDefault)
        {
            GHXMLNode::NodeList::const_iterator propNodeIter;
            for (propNodeIter = propsNode->getChildren().begin(); propNodeIter != propsNode->getChildren().end(); ++propNodeIter)
            {
                GHProperty* prop = (GHProperty*)mObjFactory.load(*(*propNodeIter), &extraData);
                if (prop) {
                    propVals.push_back(*prop);
                    delete prop;
                }
            }
            if (!propVals.size()) {
                GHDebugMessage::outputString("No valid properties listed for switch");
                continue;
            }   
        }
        
        std::vector<GHGUIWidgetResource*> widgets;
        GHXMLNode::NodeList::const_iterator propNodeIter;
        if (widgetsNode)
        {
            for (propNodeIter = widgetsNode->getChildren().begin(); propNodeIter != widgetsNode->getChildren().end(); ++propNodeIter)
            {
                GHGUIWidgetResource* widget = (GHGUIWidgetResource*)mObjFactory.load(*(*propNodeIter), &extraData);
                if (widget) {
                    widgets.push_back(widget);
                }
            }
        }
        std::vector<GHTransition*> transitions;
        if (transitionsNode)
        {
            for (propNodeIter = transitionsNode->getChildren().begin(); propNodeIter != transitionsNode->getChildren().end(); ++propNodeIter)
            {
                GHTransition* trans = (GHTransition*)mObjFactory.load(*(*propNodeIter), &extraData);
                if (trans) {
                    transitions.push_back(trans);
                }
            }
        }

        if (isDefault)
        {
            for (size_t widgetIdx = 0; widgetIdx < widgets.size(); ++widgetIdx)
            {
                GHGUIWidgetTransition* widgetTrans = new GHGUIWidgetTransition(widgets[widgetIdx], parentWidget, 0, GHGUITransitionDesc(), false);
                ret->addDefaultTransition(widgetTrans);
            }
            for (size_t transIdx = 0; transIdx < transitions.size(); ++transIdx)
            {
                ret->addDefaultTransition(transitions[transIdx]);
            }
        }
        else 
        {
            for (size_t propIdx = 0; propIdx < propVals.size(); ++propIdx)
            {
                if (widgets.size() == 0 && transitions.size() == 0)
                {
                    ret->addEmptyPropVal(propVals[propIdx]);
                }
                
                for (size_t widgetIdx = 0; widgetIdx < widgets.size(); ++widgetIdx)
                {
                    GHGUIWidgetTransition* widgetTrans = new GHGUIWidgetTransition(widgets[widgetIdx], parentWidget, 0, GHGUITransitionDesc(), false);
                    ret->addTransition(propVals[propIdx], widgetTrans);
                }
                for (size_t transIdx = 0; transIdx < transitions.size(); ++transIdx)
                {
                    ret->addTransition(propVals[propIdx], transitions[transIdx]);
                }
            }
        }
    }
}
