// Copyright 2010 Golden Hammer Software
#pragma once

#include "GHUtils/GHMessageHandler.h"
#include "GHString/GHString.h"
#include "GHGUIWidget.h"
#include "GTPosDescConverter.h"
#include "GTMoveType.h"

class GHEventMgr;
class GHGUIMgr;
class GHXMLObjFactory;
class GHXMLNode;
class GHPropertyContainer;
class GTUtil;
class GTMetadataList;

class GTGUINavigator : public GHMessageHandler
{
public:
	GTGUINavigator(GHEventMgr& eventMgr, 
					GHGUIWidgetResource& topLevelGUI,
					GHGUIWidgetResource*& currentSelectionPtr, 
					GTUtil& util,
					GTMetadataList& metadataList,
					GHPropertyContainer& properties,
					const GHGUIRectMaker& rectMaker,
					const GHScreenInfo& screenInfo);
	~GTGUINavigator(void);

	virtual void handleMessage(const GHMessage& message);

private:
	void createPanel(bool asChild);
	void deletePanel(void);
	bool selectChildren(GHGUIWidgetResource& panel, const GHPoint2& selectPt);
	bool selectPanel(GHGUIWidgetResource& panel, const GHPoint2& selectPt);
	void movePanel(const GHPoint2& delta);
	void bringSelectionToFront(void);

	GHGUIWidgetResource* findCurrentParent(GHGUIWidgetResource& potentialParent);
	void pushCreation(GHGUIWidgetResource* parent);
	void pushDeletion(GHGUIWidgetResource* parent);

private:
	struct MoveData
	{
		//index 0 for horizontal, 1 for vertical
		// -1 for left or up, 1 for right or down, 0 for no edge held
		GHPoint2i edgeHeld;
		bool shiftPressed : 1;

		MoveData() : edgeHeld(0, 0), shiftPressed(false) { }
		operator bool() const { return edgeHeld[0] || edgeHeld[1]; }
		bool topHeld(void) const { return edgeHeld[1] == -1; }
		bool botHeld(void) const { return edgeHeld[1] == 1; }
		bool leftHeld(void) const { return edgeHeld[0] == -1; }
		bool rightHeld(void) const { return edgeHeld[0] == 1; }
	};

private:
	GHEventMgr& mEventMgr;
	GHGUIWidgetResource& mTopPanel;
	GHGUIWidgetResource*& mCurrentPanel;
	GHPropertyContainer& mProperties;
	GTPosDescConverter mPosDescConverter;
	GTUtil& mUtil;
	GTMetadataList& mMetadataList;
	MoveData mMoveData;
	GHIdentifier mIdForAllTexts;
	int mMoveType;

	GHGUIPosDesc mPosBeforeMove;
};
