// Copyright Golden Hammer Software
#pragma once

#include "GHXMLObjLoader.h"

class GHPropertyContainer;
class GHStringIdFactory;
class GHGUIPanelXMLLoader;
class GHGUIRenderable;
class GHGUIRectMaker;
class GHInputState;
class GHControllerMgr;
class GHInputClaim;

// <guiJoystick activeProp=JOY1ACTIVE xProp=JOY1X yProp=JOY1Y maxDiff=0.3 allowedPointers="1 2 3 4">
//	<joybg>(widget)</joybg>
//	<joyfg>(widget)</joyfg>
// </guiJoystick>
class GHGUIJoystickXMLLoader : public GHXMLObjLoader
{
public:
	GHGUIJoystickXMLLoader(GHGUIRenderable& renderer, const GHGUIRectMaker& rectMaker, 
		const GHGUIPanelXMLLoader& panelLoader, GHPropertyContainer& props, const GHStringIdFactory& hashTable,
		GHInputState& inputState, GHInputClaim& pointerClaim, GHControllerMgr& controllerMgr,
		const GHXMLObjFactory& xmlFactory);

	virtual void* create(const GHXMLNode& node, GHPropertyContainer& extraData) const;
	GH_NO_POPULATE

private:
	GHGUIRenderable& mRenderer;
	const GHGUIRectMaker& mRectMaker;
	const GHGUIPanelXMLLoader& mPanelLoader;
	GHPropertyContainer& mProps;
	const GHStringIdFactory& mIdFactory;
	GHInputState& mInputState;
	GHInputClaim& mPointerClaim;
	GHControllerMgr& mControllerMgr;
	const GHXMLObjFactory& mXMLFactory;
};

