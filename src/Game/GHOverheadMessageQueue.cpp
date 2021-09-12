// Copyright Golden Hammer Software
#include "GHOverheadMessageQueue.h"
#include "GHFontRenderer.h"
#include "GHFont.h"
#include "GHGeometry.h"
#include "GHMath/GHFloat.h"
#include "GHModel.h"
#include "GHEntity.h"
#include "GHGeoCreationUtil.h"
#include "GHPlatform/GHStlUtil.h"
#include "GHPlatform/GHTimeVal.h"
#include <stdarg.h>
#include "GHMaterial.h"
#include "GHGeometryRenderable.h"
#include "GHRenderGroup.h"
#include "GHEntityHashes.h"

static const int sFormatBufSz = 50;
GHOverheadMessageQueue::GHOverheadMessageQueue(GHRenderGroup& renderGroup,
											   GHEntity& entity,
											   const GHTimeVal& timeVal,
											   const GHFontRenderer& fontRenderer,
											   GHVBFactory& vbFactory,
											   GHFont* font,
											   float charHeight)
: mRenderGroup(renderGroup)
, mEntity(entity)
, mTimeVal(timeVal)
, mFontRenderer(fontRenderer)
, mFont(0)
, mCharHeight(charHeight)
, mVbFactory(vbFactory)
, mFormattingBuffer(new char[sFormatBufSz])
, mDelayBetweenMessages(.25f)
, mLastMessageSpawnTime(0)
{
	mConfig.mMessageAscensionSpeed = 2.5f;
	mConfig.mMessageFadeSpeed = 1.f;
	mConfig.mMessageKillTime = 1.f;

	GHRefCounted::changePointer((GHRefCounted*&)mFont, font);

	mRenderable = new GHGeometryRenderable;

	//queueNewMessage("Radical!");
}

GHOverheadMessageQueue::~GHOverheadMessageQueue(void)
{
	delete mRenderable;

	GHRefCounted::changePointer((GHRefCounted*&)mFont, 0);
	delete[] mFormattingBuffer;
	killAllMessages();
}

void GHOverheadMessageQueue::killAllMessages(void)
{
	GHStlUtil::deletePointerList(mMessageQueue);
	while(mMessagesToQueue.size()){
		delete mMessagesToQueue.front();
		mMessagesToQueue.pop();
	}
}

void GHOverheadMessageQueue::queueNewMessage(const char* newMessage, ...)
{
	va_list args;
	va_start(args, newMessage);
	vsnprintf(mFormattingBuffer, sFormatBufSz, newMessage, args);
	va_end(args);
	
	GHPoint2 maxDimensions(0, 0);
	GHPoint2 upperLeft(0, 0);
	GHPoint2 lowerRight = upperLeft;
	lowerRight += maxDimensions;
	
	GHGeometry* geom = 0;

	GHVertexBuffer* textVB = mFontRenderer.createText(*mFont, mFormattingBuffer, mCharHeight, 1.0f, false,
													  GHAlign::A_TOP, GHAlign::A_CENTER, upperLeft, lowerRight, 0);
	GHVertexBufferPtr* textVBPtr = new GHVertexBufferPtr(textVB);
	geom = new GHGeometry;
	geom->setMaterial(mFont->getMaterial());
	
	mRenderable->addGeometry(geom);
	geom->setVB(textVBPtr);
	
	QueuedMessage* newMsg = new QueuedMessage(mConfig, geom, mEntity);
	mMessagesToQueue.push(newMsg);
}

void GHOverheadMessageQueue::update(void)
{
	if (mLastMessageSpawnTime + mDelayBetweenMessages < mTimeVal.getTime()
	   && mMessagesToQueue.size())
	{
		mMessageQueue.push_back(mMessagesToQueue.front());
		mMessagesToQueue.pop();
		mLastMessageSpawnTime = mTimeVal.getTime();
	}
	
	
	std::vector<QueuedMessage*>::iterator iter = mMessageQueue.begin();
	while(iter != mMessageQueue.end())
	{
		QueuedMessage* qm = *iter;
		if(qm->update(mTimeVal.getTimePassed()))
		{
			++iter;
		}
		else
		{
			delete qm;
			iter = mMessageQueue.erase(iter);
		}
	}
}

void GHOverheadMessageQueue::onActivate(void)
{
	mRenderGroup.addRenderable(*mRenderable, 0);
}

void GHOverheadMessageQueue::onDeactivate(void) 
{
	mRenderGroup.removeRenderable(*mRenderable, 0);
	killAllMessages();
}

GHOverheadMessageQueue::QueuedMessage::QueuedMessage(const QueuedMessageConfig& config, GHGeometry* geom, GHEntity& entity)
: mConfig(config)
, mTimeLeft(config.mMessageKillTime)
, mEntity(entity)
, mIsActive(false)
{
	geom->setTransform(&mTransformNode);
}

GHOverheadMessageQueue::QueuedMessage::~QueuedMessage(void)
{
	onDestruction();
}

void GHOverheadMessageQueue::QueuedMessage::onDestruction(void)
{
	if (!mIsActive) {
		return;
	}

	GHModel* model = mEntity.mProperties.getProperty(GHEntityHashes::MODEL);
	if (model && model->getSkeleton()) {
		model->getSkeleton()->removeChild(&mTransformNode);
	}
	mIsActive = false;
}

bool GHOverheadMessageQueue::QueuedMessage::update(float timePassed)
{	
	mTimeLeft -= timePassed;
	if(mTimeLeft <= 0)
	{
		onDestruction();
		return false;
	}	

	if (!mIsActive) 
	{
		GHModel* model = mEntity.mProperties.getProperty(GHEntityHashes::MODEL);
		if (model && model->getSkeleton()) {
			model->getSkeleton()->addChild(&mTransformNode);
		}
		mTransformNode.getLocalTransform().becomeIdentity();
		mIsActive = true;
	}
	/*
	GHPoint3 translate;
	mTransformNode.getLocalTransform().getTranslate(translate);
	translate[1] += mConfig.mMessageAscensionSpeed * timePassed;
	mTransformNode.getLocalTransform().setTranslate(translate);
	*/
	return true;
}
