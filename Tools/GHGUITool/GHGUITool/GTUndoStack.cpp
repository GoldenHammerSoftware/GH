// Copyright 2010 Golden Hammer Software
#include "GTUndoStack.h"
#include "GTIdentifiers.h"
#include "GHUtils/GHEventMgr.h"
#include "GHUtils/GHMessage.h"
#include "GHPlatform/GHDebugMessage.h"
#include "GHMath/GHFloat.h"
#include "GTMetadataList.h"
#include "GTUtil.h"
#include "GHXMLNode.h"

GTUndoStack::GTUndoStack(GHEventMgr& eventMgr, 
						 GHGUIWidgetResource& topGUI,
						 GHGUIWidgetResource*& currentSelection,
						 const GHGUIRectMaker& rectMaker,
						 GHPropertyContainer& props,
						 const GHScreenInfo& screenInfo,
						 GTMetadataList& metadataList,
						 GTUtil& util)
	: mEventMgr(eventMgr)
	, mTopGUI(topGUI)
	, mCurrentSelection(currentSelection)
	, mPosDescConverter(rectMaker, screenInfo)
	, mProps(props)
	, mMetadataList(metadataList)
	, mUtil(util)
{
	mEventMgr.registerListener(GTIdentifiers::M_CHANGE, *this);
	mEventMgr.registerListener(GTIdentifiers::M_UNDO, *this);
	mEventMgr.registerListener(GTIdentifiers::M_REDO, *this);
	mEventMgr.registerListener(GTIdentifiers::M_CLEARALLDATA, *this);
}

GTUndoStack::~GTUndoStack(void)
{
	mEventMgr.unregisterListener(GTIdentifiers::M_CLEARALLDATA, *this);
	mEventMgr.unregisterListener(GTIdentifiers::M_REDO, *this);
	mEventMgr.unregisterListener(GTIdentifiers::M_UNDO, *this);
	mEventMgr.unregisterListener(GTIdentifiers::M_CHANGE, *this);

	killStack(mUndoStack);
	killStack(mRedoStack);
}

void GTUndoStack::handleMessage(const GHMessage& message)
{
	if (message.getType() == GTIdentifiers::M_REDO)
	{
		redo();
	}
	else if (message.getType() == GTIdentifiers::M_UNDO)
	{
		undo();
	}
	else if (message.getType() == GTIdentifiers::M_CHANGE)
	{
		GTChange* change = message.getPayload().getProperty(GTIdentifiers::MP_CHANGE);
		if (change) {
			mUndoStack.push(change);
			killStack(mRedoStack);
		}
	}
	else if (message.getType() == GTIdentifiers::M_CLEARALLDATA)
	{
		killStack(mUndoStack);
		killStack(mRedoStack);
	}
}

void GTUndoStack::undo(void)
{
	if (!mUndoStack.size()) { return; }

	GTChange* change = mUndoStack.top();
	mUndoStack.pop();
	mRedoStack.push(change);

	if (change->mOldMetadata && !change->mNewMetadata)
	{
		undoDelete(*change);
	}
	else if (change->mNewMetadata && !change->mOldMetadata)
	{
		undoCreate(*change);
	}
	else
	{
		if (change->mOldMetadata->mParentName != change->mNewMetadata->mParentName)
		{
			undoParentChange(*change);
		}

		if (change->mOldMetadata->mPosDesc != change->mNewMetadata->mPosDesc)
		{
			undoPosChange(*change);
		}

		if (change->mOldMetadata->mName != change->mNewMetadata->mName)
		{
			change->mWidget->get()->setId(change->mOldMetadata->mName);
		}

		if (change->mOldMetadata->mText != change->mNewMetadata->mText)
		{
			undoTextChange(*change);
		}

		if (change->mOldMetadata->mTextParameters != change->mNewMetadata->mTextParameters)
		{
			undoTextParameterChange(*change);
		}

		setRenderable(change->mWidget, change->mOldMetadata->mRenderableNode);
	}

	if (change->mOldMetadata) {
		mMetadataList.setNode(change->mWidget, change->mOldMetadata->clone());
	}

	postResult("undid", *change);
}

void GTUndoStack::redo(void)
{
	if (!mRedoStack.size()) { return; }

	GTChange* change = mRedoStack.top();
	mRedoStack.pop();
	mUndoStack.push(change);

	if (change->mOldMetadata && !change->mNewMetadata)
	{
		redoDelete(*change);
	}
	else if (change->mNewMetadata && !change->mOldMetadata)
	{
		redoCreate(*change);
	}
	else
	{
		if (change->mOldMetadata->mParentName != change->mNewMetadata->mParentName)
		{
			redoParentChange(*change);
		}

		if (change->mOldMetadata->mPosDesc != change->mNewMetadata->mPosDesc)
		{
			redoPosChange(*change);
		}

		if (change->mOldMetadata->mName != change->mNewMetadata->mName)
		{
			change->mWidget->get()->setId(change->mNewMetadata->mName);
		}

		if (change->mOldMetadata->mText != change->mNewMetadata->mText)
		{
			redoTextChange(*change);
		}

		if (change->mOldMetadata->mTextParameters != change->mNewMetadata->mTextParameters)
		{
			redoTextParameterChange(*change);
		}

		setRenderable(change->mWidget, change->mNewMetadata->mRenderableNode);
	}

	if (change->mNewMetadata) {
		mMetadataList.setNode(change->mWidget, change->mNewMetadata->clone());
	}

	postResult("redid", *change);
}

void GTUndoStack::undoCreate(const GTChange& change)
{
	GHGUIWidgetResource* parent = findWidget(mTopGUI, change.mNewMetadata->mParentName);
	if (!parent) { parent = &mTopGUI; }

	parent->get()->removeChild(change.mWidget);

	if (change.mWidget == mCurrentSelection) {
		mCurrentSelection = 0;
	}
}

void GTUndoStack::undoDelete(const GTChange& change)
{
	GHGUIWidgetResource* parent = findWidget(mTopGUI, change.mOldMetadata->mParentName);
	if (!parent) { parent = &mTopGUI; }

	parent->get()->addChild(change.mWidget);
}

void GTUndoStack::undoPosChange(const GTChange& change)
{
	change.mWidget->get()->setPosDesc(change.mOldMetadata->mPosDesc);
	change.mWidget->get()->updatePosition();
}

void GTUndoStack::undoTextParameterChange(const GTChange& change)
{
	mUtil.setTextParameters(*change.mWidget, change.mOldMetadata->mTextParameters, 0);
}

void GTUndoStack::undoTextChange(const GTChange& change)
{
	mUtil.setText(*change.mWidget, change.mOldMetadata->mText.getChars(), 0);
}

void GTUndoStack::redoCreate(const GTChange& change)
{
	GHGUIWidgetResource* parent = findWidget(mTopGUI, change.mNewMetadata->mParentName);
	if (!parent) { parent = &mTopGUI; }
	parent->get()->addChild(change.mWidget);
}

void GTUndoStack::redoDelete(const GTChange& change)
{
	GHGUIWidgetResource* parent = findWidget(mTopGUI, change.mOldMetadata->mParentName);
	if (!parent) { parent = &mTopGUI; }
	
	parent->get()->removeChild(change.mWidget);

	if (change.mWidget == mCurrentSelection) {
		mCurrentSelection = 0;
	}
}

void GTUndoStack::redoPosChange(const GTChange& change)
{
	change.mWidget->get()->setPosDesc(change.mNewMetadata->mPosDesc);
	change.mWidget->get()->updatePosition();
}

void GTUndoStack::redoTextChange(const GTChange& change)
{
	mUtil.setText(*change.mWidget, change.mNewMetadata->mText.getChars(), 0);
}

void GTUndoStack::redoTextParameterChange(const GTChange& change)
{
	mUtil.setTextParameters(*change.mWidget, change.mNewMetadata->mTextParameters, 0);
}

void GTUndoStack::undoParentChange(const GTChange& change)
{
	GHGUIWidgetResource* widget = change.mWidget;
	GHGUIWidgetResource* oldParent = findWidget(mTopGUI, change.mOldMetadata->mParentName);
	GHGUIWidgetResource* newParent = findWidget(mTopGUI, change.mNewMetadata->mParentName);

	if (widget && oldParent)
	{
		mPosDescConverter.changeParent(*widget, newParent, *oldParent);
	}
}

void GTUndoStack::redoParentChange(const GTChange& change)
{
	GHGUIWidgetResource* widget = change.mWidget;
	GHGUIWidgetResource* oldParent = findWidget(mTopGUI, change.mOldMetadata->mParentName);
	GHGUIWidgetResource* newParent = findWidget(mTopGUI, change.mNewMetadata->mParentName);

	if (widget && newParent)
	{
		mPosDescConverter.changeParent(*widget, oldParent, *newParent);
	}
}

void GTUndoStack::setRenderable(GHGUIWidgetResource* widget, const GHXMLNode* renderableNode)
{
	if (renderableNode) {
		mUtil.setRenderableNode(*widget, *renderableNode, 0);
	}
	else {
		mUtil.setRandomColor(*widget);
	}
}

GHGUIWidgetResource* GTUndoStack::findWidget(GHGUIWidgetResource& widget, const GHIdentifier& name)
{
	if (name.isNull()) { return 0; }

	if (name == widget.get()->getId()) {
		return &widget;
	}

	return widget.get()->getChild(name);
}

void GTUndoStack::killStack(std::stack<GTChange*>& stack)
{
	while (!stack.empty())
	{
		delete stack.top();
		stack.pop();
	}
}

void GTUndoStack::postResult(const char* actionStr, const GTChange& change)
{
	const char* panelId = change.mWidget->get()->getId().getString();
	if (change.mOldMetadata && !change.mNewMetadata)
	{
		snprintf(mResultBuf, BUFSIZ, "%s deletion of %s", actionStr, panelId);
	}
	else if (change.mNewMetadata && !change.mOldMetadata)
	{
		snprintf(mResultBuf, BUFSIZ, "%s creation of %s", actionStr, panelId);
	}
	else
	{
		bool changeFound = false;

		if (change.mOldMetadata->mParentName != change.mNewMetadata->mParentName)
		{
			const char* oldParentId = change.mOldMetadata->mParentName.getString();
			const char* newParentId = change.mNewMetadata->mParentName.getString();
			snprintf(mResultBuf, BUFSZ, "%s %s's parent change from %s to %s", actionStr, panelId, oldParentId, newParentId);
			changeFound = true;
		}

		if (change.mOldMetadata->mPosDesc != change.mNewMetadata->mPosDesc)
		{
			fillResultBufWithPosChangeResult(actionStr, change);
			changeFound = true;
		}

		if (change.mOldMetadata->mName != change.mNewMetadata->mName)
		{
			snprintf(mResultBuf, BUFSZ, "%s panel id change from %s to %s", actionStr, change.mOldMetadata->mName.getString(), change.mNewMetadata->mName.getString());
			changeFound = true;
		}

		if (change.mOldMetadata->mText != change.mNewMetadata->mText)
		{
			const char* oldChars = change.mOldMetadata->mText.getChars();
			if (!oldChars) { oldChars = "nothing"; }
			const char* newChars = change.mNewMetadata->mText.getChars();
			if (!newChars) { newChars = "nothing"; }
			snprintf(mResultBuf, BUFSZ, "%s text change from %s to %s", actionStr, oldChars, newChars);
			changeFound = true;
		}
		else if (change.mOldMetadata->mTextParameters != change.mNewMetadata->mTextParameters)
		{
			fillResultBufWithTextParamChangeResult(actionStr, change);
			changeFound = true;
		}

		if (!changeFound)
		{
			fillResultBufWithRenderableDiff(actionStr, change);
		}
	}

	mProps.setProperty(GTIdentifiers::GP_CONSOLERESULTSTR, mResultBuf);
}

void GTUndoStack::fillResultBufWithPosChangeResult(const char* actionStr, const GTChange& change)
{
	if (!change.mOldMetadata || !change.mNewMetadata) {
		return;
	}

	const GHGUIPosDesc& oldDesc = change.mOldMetadata->mPosDesc;
	const GHGUIPosDesc& newDesc = change.mNewMetadata->mPosDesc;

	const char* panelId = change.mWidget->get()->getId().getString();

	if (!GHFloat::isEqual(oldDesc.mAlign, newDesc.mAlign))
	{
		snprintf(mResultBuf, BUFSIZ, "%s align change of %s", actionStr, panelId);
	}
	else if (!GHFloat::isEqual(oldDesc.mOffset, newDesc.mOffset))
	{
		snprintf(mResultBuf, BUFSIZ, "%s offset change of %s", actionStr, panelId);
	}
	else if (!GHFloat::isEqual(oldDesc.mSize, newDesc.mSize))
	{
		snprintf(mResultBuf, BUFSIZ, "%s size change of %s", actionStr, panelId);
	}
	else if (oldDesc.mSizeAlign[0] != newDesc.mSizeAlign[0])
	{
		snprintf(mResultBuf, BUFSIZ, "%s xSizeAlign change of %s", actionStr, panelId);
	}
	else if (oldDesc.mSizeAlign[1] != newDesc.mSizeAlign[1])
	{
		snprintf(mResultBuf, BUFSIZ, "%s ySizeAlign change of %s", actionStr, panelId);
	}
	else if (oldDesc.mXFill != newDesc.mXFill)
	{
		snprintf(mResultBuf, BUFSIZ, "%s xFill change of %s", actionStr, panelId);
	}
	else if (oldDesc.mYFill != newDesc.mYFill)
	{
		snprintf(mResultBuf, BUFSIZ, "%s yFill change of %s", actionStr, panelId);
	}
}

void GTUndoStack::fillResultBufWithTextParamChangeResult(const char* actionStr, const GTChange& change)
{
	if (!change.mOldMetadata || !change.mNewMetadata) {
		return;
	}
	
	auto& oldParams = change.mOldMetadata->mTextParameters;
	auto& newParams = change.mNewMetadata->mTextParameters;
	const char* panelId = change.mWidget->get()->getId().getString();

	if (oldParams.mFill != newParams.mFill)
	{
		snprintf(mResultBuf, BUFSIZ, "%s text fill type change of %s", actionStr, panelId);
	}
	else if (oldParams.mHeight != newParams.mHeight)
	{
		snprintf(mResultBuf, BUFSIZ, "%s text height change of %s", actionStr, panelId);
	}
	else if (oldParams.mHAlign != newParams.mHAlign)
	{
		snprintf(mResultBuf, BUFSIZ, "%s text xAlign change of %s", actionStr, panelId);
	}
	else if (oldParams.mVAlign != newParams.mVAlign)
	{
		snprintf(mResultBuf, BUFSIZ, "%s text yAlign change of %s", actionStr, panelId);
	}
	else if (oldParams.mWrap != newParams.mWrap)
	{
		snprintf(mResultBuf, BUFSIZ, "%s wrap change to %s", actionStr, newParams.mWrap ? "true" : "false");
	}
}

void GTUndoStack::fillResultBufWithRenderableDiff(const char* actionStr, const GTChange& change)
{
	const char* panelId = change.mWidget->get()->getId().getString();
	if (!change.mNewMetadata || !change.mNewMetadata->mRenderableNode) { return; }

	const char* newUVAngle = change.mNewMetadata->mRenderableNode->getAttribute("uvAngle");

	if (newUVAngle)
	{
		const char* oldUVAngle = 0;
		if (change.mOldMetadata && change.mOldMetadata->mRenderableNode) {
			oldUVAngle = change.mOldMetadata->mRenderableNode->getAttribute("uvAngle");
		}

		if (!oldUVAngle || strcmp(newUVAngle, oldUVAngle)) {
			snprintf(mResultBuf, BUFSIZ, "%s uvAngle change of %s", actionStr, panelId);
		}
	}


	const GHXMLNode* newMatNode = change.mNewMetadata->mRenderableNode->getChild("ghm", true);
	if (newMatNode)
	{
		const GHXMLNode* oldMatNode = 0;
		if (change.mOldMetadata && change.mOldMetadata->mRenderableNode) {
			oldMatNode = change.mOldMetadata->mRenderableNode->getChild("ghm", true);
		}

		const GHXMLNode* newTexturesNode = newMatNode->getChild("textures", false);
		if (newTexturesNode)
		{
			const GHXMLNode* oldTexturesNode = oldMatNode ? oldMatNode->getChild("textures", false) : 0;

			const char* newTexture = newTexturesNode->getChildren()[0]->getAttribute("file");
			const char* oldTexture = oldTexturesNode ? oldTexturesNode->getChildren()[0]->getAttribute("file") : 0;


			if (!(oldTexture && !strcmp(oldTexture, newTexture)))
			{
				snprintf(mResultBuf, BUFSIZ, "%s texture change of %s", actionStr, panelId);
			}
		}
		else
		{
			const GHXMLNode* newFloatArgsNode = newMatNode->getChild("floatargs", false);
			if (newFloatArgsNode)
			{
				GHPoint4 newColor;
				newFloatArgsNode->getChildren()[0]->readAttrFloatArr("value", newColor.getArr(), 4);

				const GHXMLNode* oldMatNode = 0;
				if (change.mOldMetadata && change.mOldMetadata->mRenderableNode) {
					oldMatNode = change.mOldMetadata->mRenderableNode->getChild("ghm", true);
				}

				bool colorChanged = true;
				if (oldMatNode)
				{
					const GHXMLNode* oldFloatArgsNode = oldMatNode->getChild("floatArgs", false);
					if (oldFloatArgsNode)
					{
						GHPoint4 oldColor;
						oldFloatArgsNode->getChildren()[0]->readAttrFloatArr("value", oldColor.getArr(), 4);

						if (GHFloat::isEqual(oldColor, newColor))
						{
							colorChanged = false;
						}
					}
				}

				if (colorChanged)
				{
					snprintf(mResultBuf, BUFSIZ, "%s color change of %s", actionStr, panelId);
				}
			}
		}
	}
}
