// Copyright Golden Hammer Software
#include "GHTransitionSwitchXMLLoader.h"
#include "GHUtils/GHTransitionSwitch.h"
#include "GHString/GHStringIdFactory.h"
#include "GHPlatform/GHDebugMessage.h"
#include "GHXMLNode.h"
#include "GHXMLObjFactory.h"
#include "GHUtils/GHPropertyContainer.h"

GHTransitionSwitchXMLLoader::GHTransitionSwitchXMLLoader(const GHPropertyContainer& props,
                                                         const GHStringIdFactory& hashTable,
                                                         GHXMLObjFactory& objFactory,
                                                         GHControllerMgr& controllerMgr)
: mProps(props)
, mIdFactory(hashTable)
, mObjFactory(objFactory)
, mControllerMgr(controllerMgr)
{
}

void* GHTransitionSwitchXMLLoader::create(const GHXMLNode& node, GHPropertyContainer& extraData) const
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

void GHTransitionSwitchXMLLoader::populate(void* obj, const GHXMLNode& node, GHPropertyContainer& extraData) const
{
    GHTransitionSwitch* ret = (GHTransitionSwitch*)obj;
    
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
        const GHXMLNode* widgetsNode = switchChild->getChild("transitions", false);
        if (!widgetsNode) {
            GHDebugMessage::outputString("switch with no transitions node specified");
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
        
        std::vector<GHTransition*> widgets;
        GHXMLNode::NodeList::const_iterator propNodeIter;
        for (propNodeIter = widgetsNode->getChildren().begin(); propNodeIter != widgetsNode->getChildren().end(); ++propNodeIter)
        {
            GHTransition* trans = (GHTransition*)mObjFactory.load(*(*propNodeIter), &extraData);
            if (trans) {
                widgets.push_back(trans);
            }
        }
        
        if (isDefault)
        {
            for (size_t widgetIdx = 0; widgetIdx < widgets.size(); ++widgetIdx)
            {
                ret->addDefaultTransition(widgets[widgetIdx]);
            }
        }
        else
        {
            for (size_t propIdx = 0; propIdx < propVals.size(); ++propIdx)
            {
                if (widgets.size() == 0)
                {
                    ret->addEmptyPropVal(propVals[propIdx]);
                }
                
                for (size_t widgetIdx = 0; widgetIdx < widgets.size(); ++widgetIdx)
                {
                    ret->addTransition(propVals[propIdx], widgets[widgetIdx]);
                }
            }
        }
        
    }
}
