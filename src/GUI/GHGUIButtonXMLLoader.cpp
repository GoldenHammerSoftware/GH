// Copyright Golden Hammer Software
#include "GHGUIButtonXMLLoader.h"
#include "GHGUIButton.h"
#include "GHGUIPanelXMLLoader.h"
#include "GHXMLNode.h"
#include "GHStateMachineXMLLoader.h"
#include "GHUtils/GHPropertyStacker.h"
#include "GHGUIProperties.h"
#include "GHString/GHStringTokenizer.h"
#include "GHKeyDefParser.h"
#include "GHUtils/GHProfiler.h"
#include "GHWidgetStates.h"
#include "GHGUINavRegistrationTrans.h"
#include "GHUtils/GHPropertyContainer.h"
#include "GHString/GHStringIdFactory.h"
#include "GHXMLObjFactory.h"
#include "GHGUINavToIDResponse.h"
#include "GHGUINavResponseNull.h"
#include "GHGUINavMgr.h"

GHGUIButtonXMLLoader::GHGUIButtonXMLLoader(const GHXMLObjFactory& objFactory,
										   const GHGUIPanelXMLLoader& panelLoader,
                                           const GHStateMachineXMLLoader& stateMachineLoader,
                                           GHGUIRenderable& guiRenderable, 
                                           const GHGUIRectMaker& rectMaker,
                                           GHInputState& inputState,
										   GHInputClaim& inputClaim,
                                           GHControllerMgr& controllerMgr,
                                           const GHIdentifierMap<int>& enumStore,
										   const GHStringIdFactory& hashtable,
										   GHGUINavMgr& navMgr)
: mObjFactory(objFactory)
, mPanelLoader(panelLoader)
, mStateMachineLoader(stateMachineLoader)
, mGUIRenderable(guiRenderable)
, mGUIRectMaker(rectMaker)
, mInputState(inputState)
, mInputClaim(inputClaim)
, mControllerMgr(controllerMgr)
, mEnumStore(enumStore)
, mIdFactory(hashtable)
, mNavMgr(navMgr)
{
}

void* GHGUIButtonXMLLoader::create(const GHXMLNode& node, GHPropertyContainer& extraData) const
{
    GHPROFILESCOPE("GHGUIButtonXMLLoader::create", GHString::CHT_REFERENCE)

    std::vector<int> triggerKeys;
    GHGUIButton* button = new GHGUIButton(mGUIRenderable, mGUIRectMaker, mInputState, mInputClaim, mNavMgr.getActionClaim(), mControllerMgr, triggerKeys);
    GHGUIWidgetResource* ret = new GHGUIWidgetResource(button);
	button->getWidgetStates().addTransition(GHWidgetState::WS_ACTIVE, new GHGUINavRegistrationTrans(mNavMgr, *button));
    button->setOwnerResource(ret);

    populate(ret, node, extraData);
    return ret;
}

void GHGUIButtonXMLLoader::populate(void* obj, const GHXMLNode& node, GHPropertyContainer& extraData) const
{
    GHPROFILESCOPE("GHGUIButtonXMLLoader::populate", GHString::CHT_REFERENCE)
    
    GHGUIWidgetResource* res = (GHGUIWidgetResource*)obj;
    GHGUIButton* ret = (GHGUIButton*)res->get();
    
    mPanelLoader.populate(res, node, extraData);

    GHPropertyStacker parentStack(extraData, GHGUIProperties::GP_PARENTWIDGET, GHProperty(ret));
    GHPropertyStacker parentResStack(extraData, GHGUIProperties::GP_PARENTWIDGETRES, GHProperty(res));

    const GHXMLNode* buttonStatesNode = node.getChild("buttonStates", false);
    if (buttonStatesNode) {
        mStateMachineLoader.populate(&ret->getButtonStates(), *buttonStatesNode, extraData);
    }
    
    const char* triggerKeysString = node.getAttribute("triggerKeys");
    if (triggerKeysString)
    {
        GHKeyDefParser parser(mEnumStore);
        
        GHStringTokenizer strTok;
        const char* token;
        int tokenLen;
        while (strTok.popToken(triggerKeysString, ' ', token, tokenLen, triggerKeysString))
        {
            GHKeyDef::Enum triggerKey;
            if (parser.parseString(token, triggerKey, tokenLen))
            {
                ret->addTriggerKey(triggerKey);
            }
        }        
    }

	bool isNavigable = true;
	node.readAttrBool("navigable", isNavigable);
	ret->getNavigator().setNavigable(isNavigable);

	if (isNavigable)
	{
		const GHXMLNode* navNode = node.getChild("navigation", false);
		if (navNode)
		{
			loadNavigator(*ret, *navNode, extraData);
		}
	}
}

void GHGUIButtonXMLLoader::loadNavigator(GHGUIButton& ret, const GHXMLNode& node, GHPropertyContainer& extraData) const
{
	float priority = 0;
	node.readAttrFloat("priority", priority);
	ret.getNavigator().setPriority(priority);

	loadNavigatorDirResponse(ret.getNavigator().mUpResponse, "up", "upResponse", node, extraData);
	loadNavigatorDirResponse(ret.getNavigator().mDownResponse, "down", "downResponse", node, extraData);
	loadNavigatorDirResponse(ret.getNavigator().mLeftResponse, "left", "leftResponse", node, extraData);
	loadNavigatorDirResponse(ret.getNavigator().mRightResponse, "right", "rightResponse", node, extraData);

	const GHXMLNode* eventNode = node.getChild("selectResponse", false);
	if (eventNode && eventNode->getChildren().size())
	{
		const GHXMLNode* responseNode = eventNode->getChildren()[0];
		ret.getNavigator().mSelectResponse = (GHGUINavResponse*)mObjFactory.load(*responseNode, &extraData);
	}
}

void GHGUIButtonXMLLoader::loadNavigatorDirResponse(GHGUINavResponse*& ret, const char* idPropKey, const char* eventPropKey, const GHXMLNode& node, GHPropertyContainer& extraData) const
{
	const GHXMLNode* idNode = node.getChild(idPropKey, false);
	if (idNode)
	{
		GHIdentifier identifier;
		if (idNode->readAttrIdentifier("id", identifier, mIdFactory))
		{
			delete ret;
			ret = new GHGUINavToIDResponse(mNavMgr, identifier);
		}
	}
	else
	{
		const GHXMLNode* eventNode = node.getChild(eventPropKey, false);
		if (eventNode && eventNode->getChildren().size())
		{
			const GHXMLNode* responseNode = eventNode->getChildren()[0];

			delete ret;
			if (responseNode->getName() == "0")
			{
				ret = new GHGUINavResponseNull;
			}
			else
			{
				ret = (GHGUINavResponse*)mObjFactory.load(*responseNode, &extraData);
			}
		}
	}
}
