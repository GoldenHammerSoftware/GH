// Copyright Golden Hammer Software
#pragma once

#include <vector>
#include "GHGUIWidget.h"
#include "GHUtils/GHMessageHandler.h"

class GHXMLSerializer;
class GHXMLObjFactory;
class GHPropertyContainer;
class GHXMLNode;
class GHEventMgr;

// Keeps track of which news items have been displayed.
// we load a ghnewscfg.xml that ships with the app to get the news items
// we load/save a ghnewssave.xml to say whether items have been read yet.
class GHNewsMgr
{
public:
	GHNewsMgr(GHXMLSerializer& xmlSerializer, 
			  GHXMLObjFactory& objFactory, 
			  GHPropertyContainer& guiProps,
			  GHEventMgr& eventMgr);
	~GHNewsMgr(void);

	void addNewsToGUI(GHGUIWidget& parent);
	void removeNewsFromGUI(GHGUIWidget& parent);

	void handleNewsUpdateMessage(const GHXMLNode& node);

private:
	void updateUnreadProperty(void);

	void loadNewsCfg(void);
	void loadNewsCfg(const GHXMLNode& node);
	void loadNewsItem(const GHXMLNode& node);
	bool containsNewItems(const GHXMLNode& node) const;

	void loadNewsSave(void);
	void saveNewsSave(void);

private:
	class NewsItem
	{
	public:
		NewsItem(void);
		NewsItem(const NewsItem& other);
		~NewsItem(void);

		void setGUIWidget(GHGUIWidgetResource* widget);
		GHGUIWidgetResource* getGUIWidget(void);

	public:
		int mId;
		bool mViewed;
		// flag to say whether we count this as read if this is a fresh install.
		// if true we set it to viewed if we don't find a ghnewssave.xml
		// if false we leave it as unviewed.
		bool mClearIfNewInstall;

	private:
		GHGUIWidgetResource* mWidget;
	};

	class MessageHandler : public GHMessageHandler
	{
	public:
		MessageHandler(GHNewsMgr& parent);

		virtual void handleMessage(const GHMessage& message);
	private:
		GHNewsMgr& mParent;
	} mMessageHandler;

private:
	GHXMLSerializer& mXMLSerializer;
	GHXMLObjFactory& mXMLObjFactory;
	GHPropertyContainer& mGUIProps;
	GHEventMgr& mEventMgr;

	std::vector<NewsItem> mNewsItems;
};
