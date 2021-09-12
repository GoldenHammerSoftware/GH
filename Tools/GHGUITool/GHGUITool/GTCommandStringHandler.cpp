// Copyright 2010 Golden Hammer Software
#include "GTCommandStringHandler.h"
#include "GHGUIWidget.h"
#include "GHString/GHStringTokenizer.h"
#include "GHUtils/GHMessage.h"
#include "GHUtils/GHMessageHandler.h"
#include "GTIdentifiers.h"
#include "GTChange.h"
#include "GHMath/GHFloat.h"
#include "GHString/GHStringIdFactory.h"
#include "GTUtil.h"
#include "GHPlatform/GHFilePicker.h"
#include "GHUtils/GHUtilsIdentifiers.h"
#include "GTUtil.h"
#include "GTChangePusher.h"
#include "GTMoveType.h"
#include "GHGUIText.h"

GTCommandStringHandler::GTCommandStringHandler(GHGUIWidgetResource& topGUI,
												GHGUIWidgetResource*& selectedGUI,
												const GHIdentifierMap<int>& enumStore,
												GHPropertyContainer& props,
												const GHGUIRectMaker& rectMaker,
												const GHScreenInfo& screenInfo,
												GHMessageHandler& messageHandler,
												const GHStringIdFactory& idFactory,
												const GHFilePicker& filePicker,
												GTUtil& util,
												GTMetadataList& metadataList)
: mTopGUI(topGUI)
, mSelectedGUI(selectedGUI)
, mEnumStore(enumStore)
, mProps(props)
, mPosDescConverter(rectMaker, screenInfo)
, mMessageHandler(messageHandler)
, mIdFactory(idFactory)
, mFilePicker(filePicker)
, mUtil(util)
, mMetadataList(metadataList)
{

}


void GTCommandStringHandler::parseCommand(const char* command, char* resultBuffer, size_t resultBufLen)
{
	bool handled = false;

	struct CommandParse
	{
		typedef bool (GTCommandStringHandler::*ptrHandleCommand)(const char*);
		GHString mCommand;
		ptrHandleCommand mFunc;
		bool mPushChange;
		CommandParse(const char* command, ptrHandleCommand func, bool pushChange=true)
			: mFunc(func), mPushChange(pushChange)
		{
			mCommand.setConstChars(command, GHString::CHT_REFERENCE);
		}
	};

	CommandParse commandList[] =
	{
		CommandParse("size=", &GTCommandStringHandler::setSize),
		CommandParse("height=", &GTCommandStringHandler::setHeight),
		CommandParse("width=", &GTCommandStringHandler::setWidth),
		CommandParse("align=", &GTCommandStringHandler::setAlign),
		CommandParse("offset=", &GTCommandStringHandler::setOffset),
		CommandParse("xfill=", &GTCommandStringHandler::setXFill),
		CommandParse("yfill=", &GTCommandStringHandler::setYFill),
		CommandParse("xsizealign=", &GTCommandStringHandler::setXSizeAlign),
		CommandParse("ysizealign=", &GTCommandStringHandler::setYSizeAlign),
		CommandParse("color=", &GTCommandStringHandler::setColor),
		CommandParse("parent=", &GTCommandStringHandler::setParent, false),
		CommandParse("texture=", &GTCommandStringHandler::setTexture),
		CommandParse("id=", &GTCommandStringHandler::setId),
		CommandParse("uvangle=", &GTCommandStringHandler::setUVAngle, false),
		CommandParse("movetype=", &GTCommandStringHandler::setMoveType, false),
		CommandParse("text=", &GTCommandStringHandler::setText, false),
		CommandParse("theight=", &GTCommandStringHandler::setTextHeight, false),
		//Todo: automatically recalculate textheight when textheightfill is changed (the same way we do for xfill and yfill)
		CommandParse("tfill=", &GTCommandStringHandler::setTextHeightFill, false),
		CommandParse("txalign=", &GTCommandStringHandler::setTextHAlign, false),
		CommandParse("tyalign=", &GTCommandStringHandler::setTextVAlign, false),
		CommandParse("twrap=", &GTCommandStringHandler::setTextWrap, false),
	};

	GHGUIPosDesc posBeforeHandle;
	if (mSelectedGUI) {
		posBeforeHandle = mSelectedGUI->get()->getPosDesc();
	}

	//This for loop may be confusing because of all the pointer indirection.
	// Essentially, we're looping through all of the commands that we support,
	// if our input command matches one of them, then we call the corresponding
	// function on our handler. We pass to the handler the rest of the string
	// that comes after the command string that we just used to determine
	// which function to call. This rest of the string contains any
	// parameters.
	bool pushChange = false;
	for (int i = 0; i < sizeof(commandList) / sizeof(commandList[0])
		&& !handled; ++i)
	{
		CommandParse& cmd = commandList[i];
		if (!::strncmp(cmd.mCommand, command, cmd.mCommand.getCharLen()))
		{
			handled = (this->*cmd.mFunc)(&command[cmd.mCommand.getCharLen()]);
			if (handled) { pushChange = cmd.mPushChange; }
		}
	}

	if (handled)
	{
		if (mSelectedGUI) 
		{
			mSelectedGUI->get()->updatePosition();
			if (pushChange) {
				handleChange(posBeforeHandle);
			}
		}
		snprintf(resultBuffer, resultBufLen, "Handled command: %s", command);
	}
	else
	{
		snprintf(resultBuffer, resultBufLen, "Unable to handle command: %s", command);
	}
}

bool GTCommandStringHandler::setSize(const char* param)
{
	if (!mSelectedGUI)  { return false; }
	GHGUIPosDesc posDesc = mSelectedGUI->get()->getPosDesc();
	readPoint(posDesc.mSize, param);
	mSelectedGUI->get()->setPosDesc(posDesc);
	return true;
}

bool GTCommandStringHandler::setHeight(const char* param)
{
	if (!mSelectedGUI)  { return false; }
	GHGUIPosDesc posDesc = mSelectedGUI->get()->getPosDesc();
	posDesc.mSize[1] = (float)atof(param);
	mSelectedGUI->get()->setPosDesc(posDesc);
	return true;
}

bool GTCommandStringHandler::setWidth(const char* param)
{
	if (!mSelectedGUI)  { return false; }
	GHGUIPosDesc posDesc = mSelectedGUI->get()->getPosDesc();
	posDesc.mSize[0] = (float)atof(param);
	mSelectedGUI->get()->setPosDesc(posDesc);
	return true;
}

bool GTCommandStringHandler::setAlign(const char* param)
{
	if (!mSelectedGUI)  { return false; }
	GHPoint2 align;
	readPoint(align, param);
	mPosDescConverter.setAlign(*mSelectedGUI->get(), align);
	return true;
}

bool GTCommandStringHandler::setOffset(const char* param)
{
	if (!mSelectedGUI)  { return false; }
	GHGUIPosDesc posDesc = mSelectedGUI->get()->getPosDesc();
	readPoint(posDesc.mOffset, param);
	mSelectedGUI->get()->setPosDesc(posDesc);
	return true;
}

bool GTCommandStringHandler::setXFill(const char* param)
{
	if (!mSelectedGUI)  { return false; }
	GHFillType::Enum xFill;
	if (getFillType(param, xFill))
	{
		mPosDescConverter.setXFill(*mSelectedGUI->get(), xFill);
		return true;
	}
	return false;
}

bool GTCommandStringHandler::setYFill(const char* param)
{
	if (!mSelectedGUI)  { return false; }
	GHFillType::Enum yFill;
	if (getFillType(param, yFill))
	{
		mPosDescConverter.setYFill(*mSelectedGUI->get(), yFill);
		return true;
	}
	return false;
}

bool GTCommandStringHandler::setXSizeAlign(const char* param)
{
	if (!mSelectedGUI)  { return false; }
	GHAlign::Enum xSizeAlign;
	if (getAlign(param, xSizeAlign))
	{
		mPosDescConverter.setXSizeAlign(*mSelectedGUI->get(), xSizeAlign);
		return true;
	}
	return false;
}

bool GTCommandStringHandler::setYSizeAlign(const char* param)
{
	if (!mSelectedGUI)  { return false; }
	GHAlign::Enum ySizeAlign;
	if (getAlign(param, ySizeAlign))
	{
		mPosDescConverter.setYSizeAlign(*mSelectedGUI->get(), ySizeAlign);
		return true;
	}
	return false;
}

bool GTCommandStringHandler::setText(const char* param)
{
	if (!mSelectedGUI) { return false; }

	GTChangePusher changePusher(mMessageHandler, mMetadataList, mSelectedGUI);
	mUtil.setText(*mSelectedGUI, param, &changePusher);
	return true;
}

bool GTCommandStringHandler::setTextHeight(const char* param)
{
	if (!mSelectedGUI) { return false; }

	GTChangePusher changePusher(mMessageHandler, mMetadataList, mSelectedGUI);
	GTMetadata::TextParameters textParameters;
	mUtil.getTextParameters(*mSelectedGUI, textParameters);
	textParameters.mHeight = (float)atof(param);
	mUtil.setTextParameters(*mSelectedGUI, textParameters, &changePusher);
	return true;
}

bool GTCommandStringHandler::setTextHeightFill(const char* param)
{
	if (!mSelectedGUI) { return false; }

	GHFillType::Enum textHeightFill;
	if (getFillType(param, textHeightFill))
	{
		GTChangePusher changePusher(mMessageHandler, mMetadataList, mSelectedGUI);
		GHGUIWidgetResource* textResource = mUtil.getTextChild(*mSelectedGUI);
		if (textResource)
		{
			GHGUIText* text = (GHGUIText*)textResource->get();
			GTMetadata::TextParameters textParameters;
			mUtil.getTextParameters(*mSelectedGUI, textParameters);
			mPosDescConverter.setTextHeightFillType(*text, textHeightFill);
			textParameters.mFill = textHeightFill;
			textParameters.mHeight = text->getTextHeight();
			mUtil.setTextParameters(*mSelectedGUI, textParameters, &changePusher);
			return true;
		}
	}
	return false;
}

bool GTCommandStringHandler::setTextHAlign(const char* param)
{
	if (!mSelectedGUI) { return false; }

	GHAlign::Enum hAlign;
	if (getAlign(param, hAlign))
	{
		GTChangePusher changePusher(mMessageHandler, mMetadataList, mSelectedGUI);
		GTMetadata::TextParameters textParameters;
		mUtil.getTextParameters(*mSelectedGUI, textParameters);
		textParameters.mHAlign = hAlign;
		mUtil.setTextParameters(*mSelectedGUI, textParameters, &changePusher);
		return true;
	}
	return false;
}

bool GTCommandStringHandler::setTextVAlign(const char* param)
{
	if (!mSelectedGUI) { return false; }

	GHAlign::Enum vAlign;
	if (getAlign(param, vAlign))
	{
		GTChangePusher changePusher(mMessageHandler, mMetadataList, mSelectedGUI);
		GTMetadata::TextParameters textParameters;
		mUtil.getTextParameters(*mSelectedGUI, textParameters);
		textParameters.mVAlign = vAlign;
		mUtil.setTextParameters(*mSelectedGUI, textParameters, &changePusher);
		return true;
	}
	return false;
}

bool GTCommandStringHandler::setTextWrap(const char* param)
{
	if (!mSelectedGUI) { return false; }

	bool wrap;
	if (param && *param == 't')
	{
		wrap = true;
	}
	else if (param && *param == 'f')
	{
		wrap = false;
	}
	else
	{
		return false; //we only accept words beginning with 't' or 'f'.
	}
	
	GTChangePusher changePusher(mMessageHandler, mMetadataList, mSelectedGUI);
	GTMetadata::TextParameters textParameters;
	mUtil.getTextParameters(*mSelectedGUI, textParameters);
	textParameters.mWrap = wrap;
	mUtil.setTextParameters(*mSelectedGUI, textParameters, &changePusher);
	return true;
}

bool GTCommandStringHandler::setColor(const char* param)
{
	GHPoint4 color;
	readPoint(color, param);
	if (GHFloat::isZero(color[3])) { color[3] = .75f; }
	GHMessage message(GTIdentifiers::M_SETCOLOR);
	message.getPayload().setProperty(GTIdentifiers::MP_COLOR, &color);
	mMessageHandler.handleMessage(message);
	return true;
}

bool GTCommandStringHandler::getFillType(const char* param, GHFillType::Enum& ret) const
{
	if (!strcmp(param, "ft-pixels")) {
		ret = GHFillType::FT_PIXELS;
		return true;
	}
	else if (!strcmp(param, "ft-pct")) {
		ret = GHFillType::FT_PCT;
		return true;
	}
	return false;
	
}

bool GTCommandStringHandler::getAlign(const char* param, GHAlign::Enum& ret) const
{
	if (!strcmp(param, "a-center")) {
		ret = GHAlign::A_CENTER;
		return true;
	}
	else if (!strcmp(param, "a-left")) {
		ret = GHAlign::A_LEFT;
		return true;
	}
	else if (!strcmp(param, "a-right")) {
		ret = GHAlign::A_RIGHT;
		return true;
	}
	else if (!strcmp(param, "a-bottom")) {
		ret = GHAlign::A_BOTTOM;
		return true;
	}
	else if (!strcmp(param, "a-top")) {
		ret = GHAlign::A_TOP;
		return true;
	}
	return false;
}

bool GTCommandStringHandler::setParent(const char* param)
{
	if (!mSelectedGUI)  { return false; }
	GHIdentifier parentId = mIdFactory.generateHash(param);
	GHGUIWidgetResource* newParent = 0;
	if (parentId == mTopGUI.get()->getId()) {
		newParent = &mTopGUI;
	}
	else {
		newParent = mTopGUI.get()->getChild(parentId);
	}
	if (!newParent) { return false; }
	
	GHGUIWidgetResource* oldParent = GTUtil::findParent(mTopGUI, *mSelectedGUI);

	if (newParent == oldParent) {
		return false;
	}
	if (newParent == mSelectedGUI) {
		return false;
	}

	mPosDescConverter.changeParent(*mSelectedGUI, oldParent, *newParent);

	GTChangePusher pusher(mMessageHandler, mMetadataList, mSelectedGUI);
	GTMetadata* metadata = pusher.createMetadataClone();
	metadata->mParentName = newParent->get()->getId();
	pusher.pushChange(metadata);

	return true;
}

bool GTCommandStringHandler::setMoveType(const char* param)
{
	if (!strcmp(param, "align"))
	{
		GHMessage message(GTIdentifiers::M_SETMOVETYPE);
		message.getPayload().setProperty(GHUtilsIdentifiers::VAL, (int)GTMoveType::ALIGN);
		mMessageHandler.handleMessage(message);
		return true;
	}
	if (!strcmp(param, "offset"))
	{
		GHMessage message(GTIdentifiers::M_SETMOVETYPE);
		message.getPayload().setProperty(GHUtilsIdentifiers::VAL, (int)GTMoveType::OFFSET);
		mMessageHandler.handleMessage(message);
		return true;
	}
	return false;
}

bool GTCommandStringHandler::setTexture(const char* param)
{
	if (!mSelectedGUI)  { return false; }

	class TexturePickerCallback : public GHFilePicker::PickedCallback
	{
	public:
		TexturePickerCallback(GHMessageHandler& messageHandler) : mMessageHandler(messageHandler) {}
		TexturePickerCallback(const TexturePickerCallback& other) : mMessageHandler(other.mMessageHandler) { }
		virtual void handleFilePicked(PickedCallback::Result& result)
		{
			GHMessage message(GTIdentifiers::M_SETTEXTURE);
			message.getPayload().setProperty(GHUtilsIdentifiers::FILEPATH, result.mFilePath.getChars());
			mMessageHandler.handleMessage(message);
		}
	private:
		GHMessageHandler& mMessageHandler;
	};
	
	std::vector<const char*> exts;
	exts.push_back(".png");
	exts.push_back(".jpg");
	exts.push_back(".pvr4");
	exts.push_back(".dds");
	mFilePicker.pickFile(TexturePickerCallback(mMessageHandler), exts);
	return true;
}

bool GTCommandStringHandler::setId(const char* param)
{
	if (!mSelectedGUI) { return false; }

	GHIdentifier newId = mIdFactory.generateHash(param);
	mSelectedGUI->get()->setId(newId);

	GTChangePusher pusher(mMessageHandler, mMetadataList, mSelectedGUI);
	GTMetadata* metadata = pusher.createMetadataClone();
	metadata->mName = newId;
	pusher.pushChange(metadata);

	return true;
}

bool GTCommandStringHandler::setUVAngle(const char* param)
{
	if (!mSelectedGUI) { return false; }


	float uvAngle = (float)atof(param);
	GTChangePusher changePusher(mMessageHandler, mMetadataList, mSelectedGUI);
	if (!mUtil.setUVAngle(*mSelectedGUI, uvAngle, changePusher)) {
		return false;
	}

	return true;
}

void GTCommandStringHandler::handleChange(const GHGUIPosDesc& prevPosDesc)
{
	const GHGUIPosDesc& newPosDesc = mSelectedGUI->get()->getPosDesc();
	if (newPosDesc != prevPosDesc)
	{
		GTChangePusher pusher(mMessageHandler, mMetadataList, mSelectedGUI);
		GTMetadata* metadata = pusher.createMetadataClone();
		metadata->mPosDesc = newPosDesc;
		pusher.pushChange(metadata);
	}
}
