// Copyright 2010 Golden Hammer Software
#include "GHConsoleCommandStringHandler.h"
#include "GHString/GHIdentifierMap.h"
#include "GHString/GHStringTokenizer.h"
#include "GHMath/GHPoint.h"
#include "GHGUIWidget.h"
#include "GTPosDescConverter.h"

class GHPropertyContainer;
class GHMessageHandler;
class GHStringIdFactory;
class GHFileOpener;
class GHFilePicker;
class GTUtil;
class GTMetadataList;

class GTCommandStringHandler : public GHConsoleCommandStringHandler::CommandParser
{
public:
	GTCommandStringHandler(GHGUIWidgetResource& topGUI,
						   GHGUIWidgetResource*& selectedGUI,
						   const GHIdentifierMap<int>& enumStore,
						   GHPropertyContainer& props,
						   const GHGUIRectMaker& rectMaker,
						   const GHScreenInfo& screenInfo,
						   GHMessageHandler& messageHandler,
						   const GHStringIdFactory& idFactory,
						   const GHFilePicker& filePicker,
						   GTUtil& util,
						   GTMetadataList& metadataList);

	virtual void parseCommand(const char* params, char* resultBuffer, size_t resultBufLen);

private:
	template<unsigned int COUNT>
	static void readPoint(GHPoint<float, COUNT>& ret, const char* param);

private:
	bool setSize(const char* param);
	bool setHeight(const char* param);
	bool setWidth(const char* param);
	bool setAlign(const char* param);
	bool setOffset(const char* param);
	bool setXFill(const char* param);
	bool setYFill(const char* param);
	bool setXSizeAlign(const char* param);
	bool setYSizeAlign(const char* param);
	bool setColor(const char* param);
	bool setParent(const char* param);
	bool setTexture(const char* param);
	bool setId(const char* param);
	bool setUVAngle(const char* param);
	bool setMoveType(const char* param);
	bool setText(const char* param);
	bool setTextHeight(const char* param);
	bool setTextHeightFill(const char* param);
	bool setTextHAlign(const char* param);
	bool setTextVAlign(const char* param);
	bool setTextWrap(const char* param);
	void handleChange(const GHGUIPosDesc& prevPosDesc);

private:
	bool getFillType(const char* param, GHFillType::Enum& ret) const;
	bool getAlign(const char* param, GHAlign::Enum& ret) const;

private:
	GHGUIWidgetResource& mTopGUI;
	GHGUIWidgetResource*& mSelectedGUI;
	const GHIdentifierMap<int>& mEnumStore;
	GHPropertyContainer& mProps;
	GHMessageHandler& mMessageHandler;
	const GHStringIdFactory& mIdFactory;
	const GHFilePicker& mFilePicker;
	GTPosDescConverter mPosDescConverter;
	GTUtil& mUtil;
	GTMetadataList& mMetadataList;
};

template <unsigned int COUNT>
void GTCommandStringHandler::readPoint(GHPoint<float, COUNT>& ret, const char* param)
{
	GHStringTokenizer strTok;
	for (size_t i = 0; i < COUNT; ++i)
	{
		if (param) {
			ret[i] = strTok.popFloat(param, GHStringTokenizer::kWhiteTxtMarkers, param);
		}
	}
}
