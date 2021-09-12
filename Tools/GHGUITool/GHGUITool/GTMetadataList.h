// Copyright 2010 Golden Hammer Software
#pragma once
#include "GHString/GHIdentifier.h"
#include <map>
#include "GHGUIPosDesc.h"
#include "GHGUIWidget.h"
#include "GHString/GHString.h"

class GHXMLNode;

struct GTMetadata
{
	GHIdentifier mName;
	GHIdentifier mParentName;
	GHGUIPosDesc mPosDesc;
	GHString mText;
	struct TextParameters{
		float mHeight{ 0 };
		GHFillType::Enum mFill{ GHFillType::FT_PCT };
		GHAlign::Enum mHAlign{ GHAlign::A_CENTER };
		GHAlign::Enum mVAlign{ GHAlign::A_CENTER };
		bool mWrap{ false };

		bool operator==(const TextParameters& other) const
		{
			return !(*this != other);
		}

		bool operator!=(const TextParameters& other) const
		{
			return memcmp(this, &other, sizeof(TextParameters)) != 0;
		}

	} mTextParameters;
	GHXMLNode* mRenderableNode;
	GTMetadata(const GHIdentifier& name, const GHIdentifier& parentName, const GHGUIPosDesc& posDesc, GHXMLNode* renderableNode, const GHString& text, const TextParameters& textParams = TextParameters()) 
		: mName(name)
		, mParentName(parentName)
		, mPosDesc(posDesc)
		, mRenderableNode(renderableNode)
		, mText(text)
		, mTextParameters(textParams)
	{}
	GTMetadata* clone(void);
	~GTMetadata(void);
};

class GTMetadataList 
{
public:

	~GTMetadataList(void);

	GTMetadata* getNode(GHGUIWidgetResource* widget);
	const GTMetadata* getNode(const GHGUIWidgetResource* widget) const;
	void setNode(GHGUIWidgetResource* widget, GTMetadata* node);
	
private:
	typedef std::map < GHGUIWidgetResource*, GTMetadata* > DataMap;

private:
	DataMap mMap;
};
