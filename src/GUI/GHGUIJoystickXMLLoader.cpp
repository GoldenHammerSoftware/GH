// Copyright Golden Hammer Software
#include "GHGUIJoystickXMLLoader.h"
#include "GHGUIJoystick.h"
#include "GHString/GHStringIdFactory.h"
#include "GHGUIPanelXMLLoader.h"
#include "GHXMLNode.h"
#include "GHXMLObjFactory.h"

GHGUIJoystickXMLLoader::GHGUIJoystickXMLLoader(GHGUIRenderable& renderer, const GHGUIRectMaker& rectMaker, 
	const GHGUIPanelXMLLoader& panelLoader, GHPropertyContainer& props, const GHStringIdFactory& hashTable,
	GHInputState& inputState, GHInputClaim& pointerClaim, GHControllerMgr& controllerMgr,
	const GHXMLObjFactory& xmlFactory)
: mPanelLoader(panelLoader)
, mRenderer(renderer)
, mRectMaker(rectMaker)
, mProps(props)
, mIdFactory(hashTable)
, mInputState(inputState)
, mPointerClaim(pointerClaim)
, mControllerMgr(controllerMgr)
, mXMLFactory(xmlFactory)
{
}

void* GHGUIJoystickXMLLoader::create(const GHXMLNode& node, GHPropertyContainer& extraData) const
{
	GHIdentifier activeProp = mIdFactory.generateHash(node.getAttribute("activeProp"));
	GHIdentifier xProp = mIdFactory.generateHash(node.getAttribute("xProp"));
	GHIdentifier yProp = mIdFactory.generateHash(node.getAttribute("yProp"));

	float maxDiff = 1.0f;
	node.readAttrFloat("maxDiff", maxDiff);

	std::vector<int> allowedPointers;
	node.readAttrIntList("allowedPointers", allowedPointers);

	GHGUIWidgetResource* joybg = 0;
	const GHXMLNode* joyBGNode = node.getChild("joybg", false);
	if (joyBGNode && joyBGNode->getChildren().size()) {
		joybg = (GHGUIWidgetResource*)mXMLFactory.load(*joyBGNode->getChildren()[0], &extraData);
	}
	GHGUIWidgetResource* joyfg = 0;
	const GHXMLNode* joyFGNode = node.getChild("joyfg", false);
	if (joyFGNode && joyFGNode->getChildren().size()) {
		joyfg = (GHGUIWidgetResource*)mXMLFactory.load(*joyFGNode->getChildren()[0], &extraData);
	}

	GHGUIJoystick* joy = new GHGUIJoystick(mRenderer, mRectMaker, mProps, 
		activeProp, xProp, yProp, maxDiff, allowedPointers,
		mInputState, mPointerClaim, mControllerMgr,
		joybg, joyfg);
	GHGUIWidgetResource* ret = new GHGUIWidgetResource(joy);
	mPanelLoader.populate(ret, node, extraData);
	return ret;
}
