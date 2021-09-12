// Copyright Golden Hammer Software
#include "GHNewsMgr.h"
#include "GHXMLSerializer.h"
#include "GHXMLObjFactory.h"
#include "GHUtils/GHPropertyContainer.h"
#include "GHPlatform/GHDebugMessage.h"
#include "GHXMLNode.h"
#include "GHGameIdentifiers.h"
#include "GHGUIWidget.h"
#include "GHUtils/GHMessage.h"
#include "GHUtils/GHEventMgr.h"
#include "GHBaseIdentifiers.h"
#include "GHUtils/GHUtilsIdentifiers.h"

GHNewsMgr::GHNewsMgr(GHXMLSerializer& xmlSerializer, GHXMLObjFactory& objFactory, GHPropertyContainer& guiProps, GHEventMgr& eventMgr)
: mMessageHandler(*this)
, mXMLSerializer(xmlSerializer)
, mXMLObjFactory(objFactory)
, mGUIProps(guiProps)
, mEventMgr(eventMgr)
{
	loadNewsCfg();
	loadNewsSave();
	updateUnreadProperty();

	mEventMgr.registerListener(GHBaseIdentifiers::M_WEBNEWSCONTENTS, mMessageHandler);
}

GHNewsMgr::~GHNewsMgr(void)
{
	mEventMgr.unregisterListener(GHBaseIdentifiers::M_WEBNEWSCONTENTS, mMessageHandler);
}

void GHNewsMgr::updateUnreadProperty(void)
{
	int numUnread = 0;
	for (size_t i = 0; i < mNewsItems.size(); ++i)
	{
		if (!mNewsItems[i].mViewed)
		{
			numUnread++;
		}
	}
	mGUIProps.setProperty(GHGameIdentifiers::GP_NUMUNREADNEWS, numUnread);
}

void GHNewsMgr::loadNewsCfg(void)
{
	GHXMLNode* newsListNode = mXMLSerializer.loadXMLFile("newscfg.xml");
	if (!newsListNode) {
		GHDebugMessage::outputString("No newscfg.xml file found");
		return;
	}

	loadNewsCfg(*newsListNode);
	delete newsListNode;
}

void GHNewsMgr::loadNewsCfg(const GHXMLNode& node)
{
	const GHXMLNode* newsListNode = &node;
	for (size_t newsIter = 0; newsIter < newsListNode->getChildren().size(); ++newsIter)
	{
		const GHXMLNode* newsNode = newsListNode->getChildren()[newsIter];
		loadNewsItem(*newsNode);
	}

	// update gui offsets so the guis don't overlap.
	float offset = 0;
	for (size_t i = 0; i < mNewsItems.size(); ++i)
	{
		GHGUIPosDesc pos = mNewsItems[i].getGUIWidget()->get()->getPosDesc();
		pos.mOffset[1] = offset + pos.mOffset[1];
		mNewsItems[i].getGUIWidget()->get()->setPosDesc(pos);

		offset += pos.mOffset[1];
		offset += pos.mSize[1];
	}
}

void GHNewsMgr::handleNewsUpdateMessage(const GHXMLNode& node)
{
	if (containsNewItems(node))
	{
		std::vector<int> viewedNews;
		for (size_t i = 0; i < mNewsItems.size(); ++i)
		{
			if (mNewsItems[i].mViewed) {
				viewedNews.push_back(mNewsItems[i].mId);
			}
		}

		mXMLSerializer.writeXMLFile("newscfg.xml", node);
		mNewsItems.clear();
		loadNewsCfg(node);

		for (size_t i = 0; i < mNewsItems.size(); ++i)
		{
			if (std::find(viewedNews.begin(), viewedNews.end(), mNewsItems[i].mId) != viewedNews.end())
			{
				mNewsItems[i].mViewed = true;
			}
		}

		updateUnreadProperty();
	}
}

bool GHNewsMgr::containsNewItems(const GHXMLNode& node) const
{
	if (!node.getChildren().size()) {
		return false;
	}

	for (size_t newsIter = 0; newsIter < node.getChildren().size(); ++newsIter)
	{
		const GHXMLNode* newsNode = node.getChildren()[newsIter];
		int id;
		if (newsNode->readAttrInt("id", id))
		{
			bool foundId = false;
			for (size_t i = 0; i < mNewsItems.size(); ++i)
			{
				if (id == mNewsItems[i].mId)
				{
					foundId = true;
					break;
				}
			}
			if (!foundId)
			{
				return true;
			}
		}
	}
	
	//For every new item, there was at least one existing news item with the same id
	return false;
}

void GHNewsMgr::loadNewsItem(const GHXMLNode& node)
{
	if (!node.getChildren().size()) {
		GHDebugMessage::outputString("News item with no gui widget");
		return;
	}
	GHGUIWidgetResource* widget = (GHGUIWidgetResource*)mXMLObjFactory.load(*node.getChildren()[0]);
	if (!widget) {
		GHDebugMessage::outputString("Failed to load gui widget for news item");
		return;
	}
	NewsItem item;
	node.readAttrInt("id", item.mId);
	node.readAttrBool("clearOnNewInstall", item.mClearIfNewInstall);
	item.mViewed = false; 
	item.setGUIWidget(widget);
	mNewsItems.push_back(item);
}

void GHNewsMgr::loadNewsSave(void)
{
	GHXMLNode* newsListNode = mXMLSerializer.loadXMLFile("newssave.xml");
	if (!newsListNode) {
		// new app install, mark things as read.
		for (size_t i = 0; i < mNewsItems.size(); ++i) {
			if (mNewsItems[i].mClearIfNewInstall) {
				mNewsItems[i].mViewed = true;
			}
		}

		saveNewsSave();
		return;
	}

	for (size_t childIter = 0; childIter < newsListNode->getChildren().size(); ++childIter)
	{
		GHXMLNode* entryNode = newsListNode->getChildren()[childIter];
		bool viewed = false;
		entryNode->readAttrBool("viewed", viewed);
		int id = 0;
		entryNode->readAttrInt("id", id);

		for (size_t i = 0; i < mNewsItems.size(); ++i)
		{
			if (mNewsItems[i].mId == id) {
				mNewsItems[i].mViewed = viewed;
			}
		}
	}

	delete newsListNode;
}

void GHNewsMgr::saveNewsSave(void)
{
	GHXMLNode topNode;
	topNode.setName("newsSave", GHString::CHT_REFERENCE);

	char buff[16];
	for (size_t i = 0; i < mNewsItems.size(); ++i) {
		GHXMLNode* itemNode = new GHXMLNode;
		itemNode->setName("newsSaveItem", GHString::CHT_REFERENCE);
		sprintf(buff, "%d", mNewsItems[i].mId);
		itemNode->setAttribute("id", GHString::CHT_REFERENCE, buff, GHString::CHT_COPY);
		if (mNewsItems[i].mViewed) {
			itemNode->setAttribute("viewed", GHString::CHT_REFERENCE, "true", GHString::CHT_REFERENCE);
		}
		else {
			itemNode->setAttribute("viewed", GHString::CHT_REFERENCE, "false", GHString::CHT_REFERENCE);
		}
		topNode.addChild(itemNode);
	}

	mXMLSerializer.writeXMLFile("newssave.xml", topNode);
}

void GHNewsMgr::addNewsToGUI(GHGUIWidget& parent)
{
	bool viewedChanged = false;

	for (size_t i = 0; i < mNewsItems.size(); ++i)
	{
		parent.addChild(mNewsItems[i].getGUIWidget());

		if (mNewsItems[i].mViewed == false)
		{
			mNewsItems[i].mViewed = true;
			viewedChanged = true;
		}
	}
	if (viewedChanged)
	{
		updateUnreadProperty();
		saveNewsSave();
	}
}

void GHNewsMgr::removeNewsFromGUI(GHGUIWidget& parent)
{
	for (size_t i = 0; i < mNewsItems.size(); ++i)
	{
		parent.removeChild(mNewsItems[i].getGUIWidget());
	}
}

GHNewsMgr::NewsItem::NewsItem(void)
: mId(0)
, mViewed(false)
, mClearIfNewInstall(true)
, mWidget(0)
{
}

GHNewsMgr::NewsItem::NewsItem(const NewsItem& other)
: mId(other.mId)
, mViewed(other.mViewed)
, mClearIfNewInstall(other.mClearIfNewInstall)
, mWidget(0)
{
	setGUIWidget(other.mWidget);
}

GHNewsMgr::NewsItem::~NewsItem(void)
{
	if (mWidget) mWidget->release();
}

void GHNewsMgr::NewsItem::setGUIWidget(GHGUIWidgetResource* widget)
{
	if (mWidget == widget) return;
	if (mWidget) mWidget->release();
	widget->acquire();
	mWidget = widget;
}

GHGUIWidgetResource* GHNewsMgr::NewsItem::getGUIWidget(void)
{
	return mWidget;
}

GHNewsMgr::MessageHandler::MessageHandler(GHNewsMgr& parent)
	: mParent(parent)
{

}

void GHNewsMgr::MessageHandler::handleMessage(const GHMessage& message)
{
	if (message.getType() == GHBaseIdentifiers::M_WEBNEWSCONTENTS)
	{
		const GHXMLNode* xmlNode = message.getPayload().getProperty(GHUtilsIdentifiers::VAL);
		if (xmlNode)
		{
			mParent.handleNewsUpdateMessage(*xmlNode);
		}
	}
}
