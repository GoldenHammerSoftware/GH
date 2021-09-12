// Copyright Golden Hammer Software
#pragma once

#include "GHXMLObjLoader.h"
#include "GHString/GHIdentifierMap.h"

class GHGUIPanelXMLLoader;
class GHStateMachineXMLLoader;
class GHGUIRenderable;
class GHGUIRectMaker;
class GHInputState;
class GHControllerMgr;
class GHInputClaim;
class GHGUINavMgr;
class GHStringIdFactory;
class GHXMLObjFactory;
class GHGUINavResponse;
class GHGUIButton;

/* format, same as GHGUIPanel + 
 <guiButton triggerKeys="a b ~">
    <buttonStates>
        <transitions stateEnum="BE_IDLE"></transitions>
        <transitions stateEnum="BE_SELECTED"></transitions>
        <transitions stateEnum="BE_TRIGGERED"></transitions>
    </buttonStates>
	<navigation priority=0> <!-- when making a "first" selection, err towards higher values. type is float, default is 0 -->
		<up id="upButtonId"/>			<!-- These are looked for first. If they exist,  -->
		<down id="downButtonId"/>		<!--  the "event" properties will be ignored. -->
		<left id="leftButtonId"/>		<!-- The values should be button ids of other -->
		<right id="rightButtonId"/>	<!--  buttons in the surrounding gui. -->

		<upResponse><customResponseGoesHere/></upResponse>					<!-- These should be GHGUINavResponse which you want to execute -->
		<downResponse/><customResponseGoesHere/></downResponse>				<!--  instead of moving to a different button.-->
		<leftResponse/><customResponseGoesHere/></leftResponse>				<!--  set to <0/> to simply disable movement in that direction -->
		<rightResponse/><customResponseGoesHere/></rightResponse>				

		<selectResponse><customResponseGoesHere/></selectResponse>  <!-- This is also a GHGUINavResponse. It is activated when the gui is selected through the navigator. -->
	</navigation>
 </guiButton>
*/
class GHGUIButtonXMLLoader : public GHXMLObjLoader
{
public:
    GHGUIButtonXMLLoader(const GHXMLObjFactory& objFactory,
						 const GHGUIPanelXMLLoader& panelLoader,
                         const GHStateMachineXMLLoader& stateMachineLoader,
                         GHGUIRenderable& guiRenderable, 
                         const GHGUIRectMaker& rectMaker,
                         GHInputState& inputState,
						 GHInputClaim& inputClaim,
                         GHControllerMgr& controllerMgr,
                         const GHIdentifierMap<int>& enumStore,
						 const GHStringIdFactory& hashtable,
						 GHGUINavMgr& navMgr);
    
    virtual void* create(const GHXMLNode& node, GHPropertyContainer& extraData) const;
    virtual void populate(void* obj, const GHXMLNode& node, GHPropertyContainer& extraData) const;

	void loadNavigator(GHGUIButton& ret, const GHXMLNode& node, GHPropertyContainer& extraData) const;
	void loadNavigatorDirResponse(GHGUINavResponse*& ret, const char* idPropKey, const char* eventPropKey, const GHXMLNode& node, GHPropertyContainer& extraData) const;

private:
	const GHXMLObjFactory& mObjFactory;
    const GHGUIPanelXMLLoader& mPanelLoader;
    const GHStateMachineXMLLoader& mStateMachineLoader;
    GHGUIRenderable& mGUIRenderable;
    const GHGUIRectMaker& mGUIRectMaker;
    GHInputState& mInputState;
	GHInputClaim& mInputClaim;
    GHControllerMgr& mControllerMgr;
    const GHIdentifierMap<int>& mEnumStore;
	const GHStringIdFactory& mIdFactory;
	GHGUINavMgr& mNavMgr;
};
