// Copyright 2010 Golden Hammer Software
#pragma once

#include "GHUtils/GHMessageHandler.h"
#include "GTChange.h"
#include <stack>
#include "GHGUIWidget.h"
#include "GHString/GHIdentifier.h"
#include "GTPosDescConverter.h"

class GHEventMgr;
class GHPropertyContainer;
class GTMetadataList;
class GTUtil;

class GTUndoStack : public GHMessageHandler
{
public:
	GTUndoStack(GHEventMgr& eventMgr,
				GHGUIWidgetResource& topGUI,
				GHGUIWidgetResource*& currentSelection,
				const GHGUIRectMaker& rectMaker,
				GHPropertyContainer& props,
				const GHScreenInfo& screenInfo,
				GTMetadataList& metadataList,
				GTUtil& util);
	virtual ~GTUndoStack(void);
	virtual void handleMessage(const GHMessage& message);

private:
	void undo(void);
	void redo(void);
	void undoCreate(const GTChange& change);
	void undoDelete(const GTChange& change);
	void undoPosChange(const GTChange& change);
	void undoTextChange(const GTChange& change);
	void undoTextParameterChange(const GTChange& change);
	void redoCreate(const GTChange& change);
	void redoDelete(const GTChange& change);
	void redoPosChange(const GTChange& change);
	void redoTextChange(const GTChange& change);
	void redoTextParameterChange(const GTChange& change);
	void applyTextParameters(GHGUIWidgetResource& widget, const GTMetadata& newMetadata);
	void undoParentChange(const GTChange& change);
	void redoParentChange(const GTChange& change);
	void setRenderable(GHGUIWidgetResource* widget, const GHXMLNode* renderableNode);
	GHGUIWidgetResource* findWidget(GHGUIWidgetResource& widget, const GHIdentifier& name);

	void killStack(std::stack<GTChange*>& stack);

	void postResult(const char* actionStr, const GTChange& change);
	void fillResultBufWithPosChangeResult(const char* actionStr, const GTChange& change);
	void fillResultBufWithTextParamChangeResult(const char* actionStr, const GTChange& change);
	void fillResultBufWithRenderableDiff(const char* actionStr, const GTChange& change);

private:
	GHEventMgr& mEventMgr;
	GHGUIWidgetResource& mTopGUI;
	GHGUIWidgetResource*& mCurrentSelection;
	GTPosDescConverter mPosDescConverter;
	GHPropertyContainer& mProps;
	GTMetadataList& mMetadataList;
	GTUtil& mUtil;

	const static int BUFSZ = 256;
	char mResultBuf[BUFSZ];
	
	std::stack<GTChange*> mUndoStack;
	std::stack<GTChange*> mRedoStack;

};
