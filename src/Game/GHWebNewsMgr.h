// Copyright Golden Hammer Software
#pragma once

#include "GHString/GHIdentifier.h"
#include "GHString/GHString.h"
#include "GHUtils/GHMessageHandler.h"
#include "GHPlatform/GHRunnable.h"

class GHXMLSerializer;
class GHSocketMgr;
class GHThreadFactory;
class GHThread;
class GHMessageHandler;
class GHEventMgr;
class GHPropertyContainer;

// class that manages news that connects to a web site.
// we keep track of the most recently viewed version of the news page
//  and we ping a web page to read the current version.
class GHWebNewsMgr : public GHMessageHandler
{
public:
	// load in the last-read news version from a in serializationFileName.
	// in a separate thread, open a socket to webHost and download webPage.
	//  we expect webPage to have a number as the first characters.
	//  the separate thread sends versionMessageId message through mainThreadMessageQueue
	// we get that message back in the main thread.
	// if mVersionId is out of date we set hasNewsId to true.
	// we then listen for readMessageId to update lastVersionId and serialize.
	GHWebNewsMgr(const char* webHost, const char* webPage, 
				 const char* serializationFileName, 
				 const GHIdentifier& versionMessageId, const GHIdentifier& newsCfgMessageId,
				 const GHIdentifier& hasNewsId, const GHIdentifier& readMessageId,
				 GHXMLSerializer& xmlSerializer, const GHSocketMgr& socketMgr,
				 const GHThreadFactory& threadFactory, GHMessageHandler& mainThreadMessageQueue,
				 GHEventMgr& eventMgr, GHPropertyContainer& props);
	~GHWebNewsMgr(void);

	virtual void handleMessage(const GHMessage& message);

private:
	void loadXML(void);
	void saveXML(void);
	// ping the website to ask for the web version
	void checkWebVersion(void);

private:
	// pings the website to check the web version.
	class NetworkRunnable : public GHRunnable
	{
	public:
		NetworkRunnable(GHMessageHandler& responseHandler, const GHSocketMgr& socketMgr,
					    const GHString& webHost, const GHString& webPage, 
						const GHIdentifier& versionMessageId, const GHIdentifier& newsCfgMessageId,
					    const GHThreadFactory& threadFactory, const GHXMLSerializer& xmlSerializer);
		virtual void run(void);
	
	private:
		GHMessageHandler& mResponseHandler;
		const GHSocketMgr& mSocketMgr;
		const GHString& mWebHost;
		const GHString& mWebPage;
		const GHIdentifier mVersionMessageId;
		const GHIdentifier mNewsCfgMessageId;
		const GHThreadFactory& mThreadFactory;
		const GHXMLSerializer& mXMLSerializer;
	};

private:
	GHXMLSerializer& mXMLSerializer;
	const GHSocketMgr& mSocketMgr;
	const GHThreadFactory& mThreadFactory;
	GHMessageHandler& mMainThreadMessageQueue;
	GHEventMgr& mEventMgr;
	GHPropertyContainer& mProps;

	NetworkRunnable* mNetworkRunnable;
	GHThread* mNetworkThread;

	// www.goldenhammersoftware.com
	GHString mHostName;
	// superbeatlinenewsversion.html
	GHString mPageName;
	// save filename
	GHString mSaveName;
	
	// storage of the last-read version
	int mLastReadNewsVersion;
	// storage of the most up-to-date news version
	int mCurrentNewsVersion;

	// a message we send from the socket thread saying version was updated.
	// we also listen for it on the main thread.
	GHIdentifier mVersionMessageId;
	// A message we send from the socket thread containing the configuration for the news, if this app supports that.
	// The app is expected to listen for this message and update the news accordingly
	GHIdentifier mNewsCfgMessageId;
	// a property we set saying unread news exists.
	GHIdentifier mHasNewsId;
	// a message we listen for to notify us that the news was read.
	GHIdentifier mReadMessageId;
};
