// Copyright Golden Hammer Software
#include "GHWebNewsMgr.h"
#include "GHUtils/GHPropertyContainer.h"
#include "GHXMLSerializer.h"
#include "GHXMLNode.h"
#include "GHUtils/GHEventMgr.h"
#include "GHUtils/GHMessage.h"
#include "GHUtils/GHUtilsIdentifiers.h"
#include "GHPlatform/GHSocketMgr.h"
#include "GHPlatform/GHSocket.h"
#include "GHPlatform/GHThreadFactory.h"
#include "GHPlatform/GHThread.h"
#include "GHPlatform/GHThreadSleeper.h"
#include "GHPlatform/GHDebugMessage.h"
#include "GHPlatform/GHConnectionType.h"

GHWebNewsMgr::GHWebNewsMgr(const char* webHost, const char* webPage,
						   const char* serializationFileName, 
						   const GHIdentifier& versionMessageId, const GHIdentifier& newsCfgMessageId,
						   const GHIdentifier& hasNewsId, const GHIdentifier& readMessageId,
						   GHXMLSerializer& xmlSerializer, const GHSocketMgr& socketMgr,
						   const GHThreadFactory& threadFactory, GHMessageHandler& mainThreadMessageQueue,
						   GHEventMgr& eventMgr, GHPropertyContainer& props)
: mHostName(webHost, GHString::CHT_COPY)
, mPageName(webPage, GHString::CHT_COPY)
, mSaveName(serializationFileName, GHString::CHT_COPY)
, mVersionMessageId(versionMessageId)
, mNewsCfgMessageId(newsCfgMessageId)
, mHasNewsId(hasNewsId)
, mReadMessageId(readMessageId)
, mXMLSerializer(xmlSerializer)
, mSocketMgr(socketMgr)
, mThreadFactory(threadFactory)
, mMainThreadMessageQueue(mainThreadMessageQueue)
, mEventMgr(eventMgr)
, mProps(props)
, mLastReadNewsVersion(0)
, mCurrentNewsVersion(0)
, mNetworkRunnable(0)
, mNetworkThread(0)
{
	mEventMgr.registerListener(mVersionMessageId, *this);
	mEventMgr.registerListener(mReadMessageId, *this);

	loadXML();
	if (mLastReadNewsVersion < mCurrentNewsVersion)
	{
		mProps.setProperty(mHasNewsId, 1);
	}
	else
	{
		checkWebVersion();
	}
}

GHWebNewsMgr::~GHWebNewsMgr(void)
{
	mEventMgr.unregisterListener(mVersionMessageId, *this);
	mEventMgr.unregisterListener(mReadMessageId, *this);
}

void GHWebNewsMgr::handleMessage(const GHMessage& message)
{
	if (message.getType() == mVersionMessageId)
	{
		int webVersion = (int)message.getPayload().getProperty(GHUtilsIdentifiers::INTVAL);
		if (webVersion)
		{
			mCurrentNewsVersion = webVersion;
			if (mCurrentNewsVersion > mLastReadNewsVersion)
			{
				mProps.setProperty(mHasNewsId, 1);
				saveXML();
			}
		}

		delete mNetworkThread;
		mNetworkThread = 0;
		delete mNetworkRunnable;
		mNetworkRunnable = 0;
	}
	else if (message.getType() == mReadMessageId)
	{
		mLastReadNewsVersion = mCurrentNewsVersion;
		saveXML();
		mProps.setProperty(mHasNewsId, 0);
	}
}

void GHWebNewsMgr::loadXML(void)
{
	GHXMLNode* saveNode = mXMLSerializer.loadXMLFile(mSaveName);
	if (saveNode)
	{
		saveNode->readAttrInt("lastRead", mLastReadNewsVersion);
		saveNode->readAttrInt("webVersion", mCurrentNewsVersion);
		delete saveNode;
	}
}

void GHWebNewsMgr::saveXML(void)
{
	GHXMLNode saveNode;
	saveNode.setName("webNews", GHString::CHT_REFERENCE);
	char buff[32];
	sprintf(buff, "%d", mLastReadNewsVersion);
	saveNode.setAttribute("lastRead", GHString::CHT_REFERENCE, buff, GHString::CHT_COPY);
	sprintf(buff, "%d", mCurrentNewsVersion);
	mXMLSerializer.writeXMLFile(mSaveName, saveNode);
}

void GHWebNewsMgr::checkWebVersion(void)
{
	mNetworkRunnable = new NetworkRunnable(mMainThreadMessageQueue, mSocketMgr, mHostName, mPageName, mVersionMessageId, mNewsCfgMessageId, mThreadFactory, mXMLSerializer);
	mNetworkThread = mThreadFactory.createThread(GHThread::TP_LOW);
	GHDebugMessage::outputString("About to run GHWebNewsMgr thread");
	mNetworkThread->runThread(*mNetworkRunnable);
}

GHWebNewsMgr::NetworkRunnable::NetworkRunnable(GHMessageHandler& responseHandler, const GHSocketMgr& socketMgr,
											   const GHString& webHost, const GHString& webPage,
											   const GHIdentifier& versionMessageId, const GHIdentifier& newsCfgMessageId,
											   const GHThreadFactory& threadFactory, const GHXMLSerializer& xmlSerializer)
: mResponseHandler(responseHandler)
, mSocketMgr(socketMgr)
, mWebHost(webHost)
, mWebPage(webPage)
, mVersionMessageId(versionMessageId)
, mNewsCfgMessageId(newsCfgMessageId)
, mThreadFactory(threadFactory)
, mXMLSerializer(xmlSerializer)
{
}

void GHWebNewsMgr::NetworkRunnable::run(void)
{
	GHDebugMessage::outputString("GHWebNewsMgr::NetworkRunnable::run()");

	int version = 0;
	GHXMLNode* newsCfgXML = 0;

    GHSocket* socket = mSocketMgr.createSocket(mWebHost.getChars(), "80", CT_TCP);
	if (socket)
	{
		GHThreadSleeper* sleeper = mThreadFactory.createSleeper();

		// send the http request.
		char sendBuff[4096];
		sprintf(sendBuff, "GET %s HTTP/1.0\r\n", mWebPage.getChars());
		socket->send(sendBuff, strlen(sendBuff));
		sprintf(sendBuff, "host: %s\r\n", mWebHost.getChars());
		socket->send(sendBuff, strlen(sendBuff));
		sprintf(sendBuff, "\r\n");
		socket->send(sendBuff, strlen(sendBuff));

		// wait for the http response.
		char readBuff[4096];
		size_t sizeRead = 0;
		while (true)
		{
			bool connected = socket->read(readBuff, 4096, sizeRead);
			if (sizeRead) {
				// we're expecting the first response to have our number in it.
				break;
			}
			if (!connected) {
				GHDebugMessage::outputString("Disconnected from news server without a response.");
				break;
			}
			// sleep and check back later, no response yet.
			if (sleeper) {
				sleeper->sleep(1000);
			}
		}
		
		if (sizeRead > 0)
		{
			readBuff[sizeRead-1] = 0;
		}

		// parse the response to extract the version.
		if (sizeRead > 12)
		{
			const char* versionPos = strstr(readBuff, "newsVersion=");
			if (!versionPos) {
				GHDebugMessage::outputString("Did not find newsVersion= in web news http response.");
			}
			else
			{
				versionPos += 12;
				version = ::atoi(versionPos);
			}

			const char* newsString = strstr(readBuff, "<newscfg>");
			if (newsString)
			{
				newsCfgXML = mXMLSerializer.loadXMLFromString(newsString);
			}
		}

		if (sleeper) delete sleeper;
		delete socket;
	}

	GHMessage versionMessage(mVersionMessageId);
	versionMessage.getPayload().setProperty(GHUtilsIdentifiers::INTVAL, version);
	mResponseHandler.handleMessage(versionMessage);

	if (newsCfgXML)
	{
		GHMessage newsCfgMessage(mNewsCfgMessageId);
		newsCfgMessage.getPayload().setProperty(GHUtilsIdentifiers::VAL, GHProperty(newsCfgXML, new GHRefCountedType<GHXMLNode>(newsCfgXML)));
		mResponseHandler.handleMessage(newsCfgMessage);
	}

	GHDebugMessage::outputString("GHWebNewsMgr::NetworkRunnable::run() finished");
}

