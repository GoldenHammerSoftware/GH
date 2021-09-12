// Copyright 2010 Golden Hammer Software
#include "GTGUINavigator.h"
#include "GTIdentifiers.h"
#include "GHUtils/GHEventMgr.h"
#include "GHUtils/GHMessage.h"
#include "GHGUIMgr.h"
#include "GHUtils/GHResourceFactory.h"
#include "GHXMLObjFactory.h"
#include "GHGUIPanel.h"
#include "GHXMLNode.h"
#include "GHGUIWidgetRenderer.h"
#include "GHUtils/GHPropertyContainer.h"
#include "GTChange.h"
#include "GHMath/GHRandom.h"
#include "GTUtil.h"
#include "GHUtils/GHUtilsIdentifiers.h"
#include "GTMetadataList.h"
#include "GTChangePusher.h"
#include "GTMoveType.h"
#include "GHMath/GHFloat.h"
#include "GHGUIText.h"

GTGUINavigator::GTGUINavigator(GHEventMgr& eventMgr,
							   GHGUIWidgetResource& topLevelGUI,
							   GHGUIWidgetResource*& currentSelectionPtr,
							   GTUtil& util,
							   GTMetadataList& metadataList,
							   GHPropertyContainer& properties,
							   const GHGUIRectMaker& rectMaker,
							   const GHScreenInfo& screenInfo)
: mEventMgr(eventMgr)
, mTopPanel(topLevelGUI)
, mCurrentPanel(currentSelectionPtr)
, mProperties(properties)
, mPosDescConverter(rectMaker, screenInfo)
, mUtil(util)
, mMetadataList(metadataList)
, mMoveType(GTMoveType::OFFSET)
{
	mEventMgr.registerListener(GTIdentifiers::M_CREATEPANEL, *this);
	mEventMgr.registerListener(GTIdentifiers::M_DELETEPANEL, *this);
	mEventMgr.registerListener(GTIdentifiers::M_SELECTPANEL, *this);
	mEventMgr.registerListener(GTIdentifiers::M_MOVEPANEL, *this);
	mEventMgr.registerListener(GTIdentifiers::M_MOVEEND, *this);
	mEventMgr.registerListener(GTIdentifiers::M_SETCOLOR, *this);
	mEventMgr.registerListener(GTIdentifiers::M_SETTEXTURE, *this);
	mEventMgr.registerListener(GTIdentifiers::M_SHIFTPRESSED, *this);
	mEventMgr.registerListener(GTIdentifiers::M_SHIFTRELEASED, *this);
	mEventMgr.registerListener(GTIdentifiers::M_SETMOVETYPE, *this);
}

GTGUINavigator::~GTGUINavigator(void)
{
	mEventMgr.unregisterListener(GTIdentifiers::M_SETMOVETYPE, *this);
	mEventMgr.unregisterListener(GTIdentifiers::M_SHIFTRELEASED, *this);
	mEventMgr.unregisterListener(GTIdentifiers::M_SHIFTPRESSED, *this);
	mEventMgr.unregisterListener(GTIdentifiers::M_SETTEXTURE, *this);
	mEventMgr.unregisterListener(GTIdentifiers::M_SETCOLOR, *this);
	mEventMgr.unregisterListener(GTIdentifiers::M_MOVEEND, *this);
	mEventMgr.unregisterListener(GTIdentifiers::M_MOVEPANEL, *this);
	mEventMgr.unregisterListener(GTIdentifiers::M_SELECTPANEL, *this);
	mEventMgr.unregisterListener(GTIdentifiers::M_DELETEPANEL, *this);
	mEventMgr.unregisterListener(GTIdentifiers::M_CREATEPANEL, *this);
}

void GTGUINavigator::handleMessage(const GHMessage& message)
{
	if (message.getType() == GTIdentifiers::M_CREATEPANEL)
	{
		bool createAsChild = message.getPayload().getProperty(GTIdentifiers::MP_CREATEASCHILD);
		createPanel(createAsChild);
	}
	else if (message.getType() == GTIdentifiers::M_DELETEPANEL)
	{
		deletePanel();
	}
	else if (message.getType() == GTIdentifiers::M_SELECTPANEL)
	{
		const GHPoint2* selectPt = (const GHPoint2*)message.getPayload().getProperty(GTIdentifiers::MP_SELECTPOINT);
		if (selectPt)
		{
			if (!selectChildren(mTopPanel, *selectPt))
			{
				mCurrentPanel = 0;
			}
		}
		bringSelectionToFront();
	}
	else if (message.getType() == GTIdentifiers::M_MOVEPANEL)
	{
		const GHPoint2* delta = (const GHPoint2*)message.getPayload().getProperty(GTIdentifiers::MP_MOVEDELTA);
		if (delta) {
			movePanel(*delta);
		}
	}
	else if (message.getType() == GTIdentifiers::M_MOVEEND)
	{
		if (mCurrentPanel)
		{
			const GHGUIPosDesc& newDesc = mCurrentPanel->get()->getPosDesc();
			if (newDesc != mPosBeforeMove)
			{
				GTChangePusher changePusher(mEventMgr, mMetadataList, mCurrentPanel);
				GTMetadata* metadata = changePusher.createMetadataClone();
				metadata->mPosDesc = newDesc;
				changePusher.pushChange(metadata);
			}
		}
	}
	else if (message.getType() == GTIdentifiers::M_SETCOLOR)
	{
		GHPoint4* color = message.getPayload().getProperty(GTIdentifiers::MP_COLOR);
		if (color && mCurrentPanel && mCurrentPanel->get())
		{
			GTChangePusher changePusher(mEventMgr, mMetadataList, mCurrentPanel);
			mUtil.setColor(*mCurrentPanel, *color, &changePusher);
		}
	}
	else if (message.getType() == GTIdentifiers::M_SETTEXTURE)
	{
		if (mCurrentPanel && mCurrentPanel->get())
		{
			const char* filename = message.getPayload().getProperty(GHUtilsIdentifiers::FILEPATH);
			GTChangePusher changePusher(mEventMgr, mMetadataList, mCurrentPanel);
			mUtil.setTexture(*mCurrentPanel, filename, changePusher);
		}
	}
	else if (message.getType() == GTIdentifiers::M_SHIFTPRESSED)
	{
		mMoveData.shiftPressed = true;
	}
	else if (message.getType() == GTIdentifiers::M_SHIFTRELEASED)
	{
		mMoveData.shiftPressed = false;
	}
	else if (message.getType() == GTIdentifiers::M_SETMOVETYPE)
	{
		int moveType = (int)message.getPayload().getProperty(GHUtilsIdentifiers::VAL);
		mMoveType = moveType;
	}
}

void GTGUINavigator::createPanel(bool asChild)
{
	GHGUIWidgetResource* parent = &mTopPanel;
	if (asChild && mCurrentPanel) {
		parent = mCurrentPanel;
	}
	
	GHGUIWidgetResource* panel = panel = mUtil.createPanel();

	//default vals
	GHGUIPosDesc posDesc;
	posDesc.mXFill = GHFillType::FT_PIXELS;
	posDesc.mYFill = GHFillType::FT_PIXELS;
	posDesc.mAlign.setCoords(.5, .5);
	posDesc.mOffset.setCoords(0, 0);
	posDesc.mSize.setCoords(300, 67.5); //menu button
	posDesc.mSizeAlign[0] = GHAlign::A_CENTER;
	posDesc.mSizeAlign[1] = GHAlign::A_CENTER;
	panel->get()->setPosDesc(posDesc);

	//GHXMLNode* matNode = createTextureMatNode("bgrect.png", GHString::CHT_REFERENCE);

	mCurrentPanel = panel;
	mUtil.setRandomColor(*mCurrentPanel);

	parent->get()->addChild(panel);
	pushCreation(parent);

	bringSelectionToFront();
}

void GTGUINavigator::deletePanel(void)
{
	if (!mCurrentPanel) { return; }

	GHGUIWidgetResource* parent = mUtil.findParent(mTopPanel, *mCurrentPanel);

	if (parent) {
		mCurrentPanel->acquire();
		parent->get()->removeChild(mCurrentPanel);
		pushDeletion(parent);
		mCurrentPanel->release();
		mCurrentPanel = 0;
	}
}

void GTGUINavigator::pushCreation(GHGUIWidgetResource* parent)
{
	GTChangePusher pusher(mEventMgr, mMetadataList, mCurrentPanel);

	GTMetadata* newData = new GTMetadata(mCurrentPanel->get()->getId(),
										 parent->get()->getId(),
										 mCurrentPanel->get()->getPosDesc(),
										 0, 0);

	pusher.pushChange(0, newData);
}

void GTGUINavigator::pushDeletion(GHGUIWidgetResource* parent)
{
	GTChangePusher pusher(mEventMgr, mMetadataList, mCurrentPanel);
	pusher.pushChange(0);
}

void GTGUINavigator::bringSelectionToFront(void)
{
	if (mCurrentPanel)
	{
		mPosBeforeMove = mCurrentPanel->get()->getPosDesc();

		GHGUIWidgetResource* parent = findCurrentParent(mTopPanel);
		if (parent) {
			mCurrentPanel->acquire();
			parent->get()->removeChild(mCurrentPanel);
			static float staticInsertionPriority = 0;
			parent->get()->addChild(mCurrentPanel, ++staticInsertionPriority);
			mCurrentPanel->release();
		}
	}
}

GHGUIWidgetResource* GTGUINavigator::findCurrentParent(GHGUIWidgetResource& potentialParent)
{
	if (!mCurrentPanel) { return 0; }

	return GTUtil::findParent(potentialParent, *mCurrentPanel);
}

bool GTGUINavigator::selectChildren(GHGUIWidgetResource& panel, const GHPoint2& selectPt)
{
	size_t numChildren = panel.get()->getNumChildren();
	for (int i = numChildren - 1; i >= 0; --i)
	{
		GHGUIWidgetResource* child = panel.get()->getChildAtIndex(i);
		if (selectPanel(*child, selectPt))
		{
			return true;
		}
	}
	return false;
}

bool GTGUINavigator::selectPanel(GHGUIWidgetResource& panel, const GHPoint2& selectPt)
{
	bool ret = false;

	//exclude guitext children from selection (they are not considered manipulatable panels in the tool)
	if (panel.get()->getId() == mUtil.getIdForAllTexts())
	{
		return false;
	}
	
	const GHRect<float, 2>& rect = panel.get()->getScreenPos();
	if (rect.containsPoint(selectPt))
	{
		const static float kLocalEps = .01f;
		MoveData moveData;
		if (rect.mMax[0] - selectPt[0] < kLocalEps) { moveData.edgeHeld[0] = 1; }
		if (rect.mMax[1] - selectPt[1] < kLocalEps) { moveData.edgeHeld[1] = 1; }
		if (selectPt[0] - rect.mMin[0] < kLocalEps) { moveData.edgeHeld[0] = -1; }
		if (selectPt[1] - rect.mMin[1] < kLocalEps) { moveData.edgeHeld[1] = -1; }
		mMoveData = moveData;
		mCurrentPanel = &panel;
		
		ret = true;
	}
	
	ret = selectChildren(panel, selectPt) || ret;
	return ret;
}

void GTGUINavigator::movePanel(const GHPoint2& delta)
{
	if (!mCurrentPanel || !mCurrentPanel->get()) { return; }

	GHGUIPosDesc posDesc = mCurrentPanel->get()->getPosDesc();

	//delta is always in fullscreen pct
	GHPoint2 convertedDelta;
	convertedDelta[0] = mPosDescConverter.convertPCTToLocalCoord(*mCurrentPanel->get(), delta[0], 0);
	convertedDelta[1] = mPosDescConverter.convertPCTToLocalCoord(*mCurrentPanel->get(), delta[1], 1);

	GHPoint2 localPctDelta;
	localPctDelta[0] = mPosDescConverter.convertPCTToLocalPCT(*mCurrentPanel->get(), delta[0], 0);
	localPctDelta[1] = mPosDescConverter.convertPCTToLocalPCT(*mCurrentPanel->get(), delta[1], 1);

	if (mMoveData)
	{
		GHPoint2i sizeAlignDir;
		sizeAlignDir[0] = posDesc.mSizeAlign[0] == GHAlign::A_LEFT ? -1 : (posDesc.mSizeAlign[0] == GHAlign::A_RIGHT ? 1 : 0);
		sizeAlignDir[1] = posDesc.mSizeAlign[1] == GHAlign::A_TOP ? -1 : (posDesc.mSizeAlign[1] == GHAlign::A_BOTTOM ? 1 : 0);

		for (int i = 0; i < 2; ++i)
		{
			if (posDesc.mSize[i] + convertedDelta[i] * mMoveData.edgeHeld[i] < 0) {
				convertedDelta[i] = -posDesc.mSize[i] * mMoveData.edgeHeld[i];
			}
			posDesc.mSize[i] += convertedDelta[i] * mMoveData.edgeHeld[i];

			localPctDelta[i] = mPosDescConverter.convertLocalCoordToLocalPCT(*mCurrentPanel->get(), convertedDelta[i], i);

			GHPoint2* ptToChange = &posDesc.mOffset;
			GHPoint2* deltaToRead = &convertedDelta;

			if (mMoveType == GTMoveType::ALIGN) {
				ptToChange = &posDesc.mAlign;
				deltaToRead = &localPctDelta;
			}

			if (mMoveData.edgeHeld[i])
			{
				if (sizeAlignDir[i] == 0) {
					(*ptToChange)[i] += (*deltaToRead)[i] * .5f;
				}
				else if (mMoveData.edgeHeld[i] != sizeAlignDir[i]) {
					(*ptToChange)[i] += (*deltaToRead)[i];
				}
			}
		}
	}
	else
	{
		if (mMoveType == GTMoveType::ALIGN) {
			posDesc.mAlign += localPctDelta;
		}
		else {
			posDesc.mOffset += convertedDelta;
		}
	}

	mCurrentPanel->get()->setPosDesc(posDesc);
	mCurrentPanel->get()->updatePosition();
}
