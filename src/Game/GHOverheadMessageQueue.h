// Copyright Golden Hammer Software
#pragma once

#include "GHUtils/GHController.h"
#include "GHTransformNode.h"
#include <queue>

class GHRenderGroup;
class GHFontRenderer;
class GHFont;
class GHVBFactory;
class GHGeometry;
class GHEntity;
class GHTimeVal;
class GHGeometryRenderable;

//Text that floats up over an entity's head and then disappears.
class GHOverheadMessageQueue : public GHController
{
public:
	
	GHOverheadMessageQueue(GHRenderGroup& renderGroup,
						   GHEntity& entity,
						   const GHTimeVal& timeVal,
						   const GHFontRenderer& fontRenderer,
						   GHVBFactory& vbFactory,
						   GHFont* font,
						   float charHeight);
	~GHOverheadMessageQueue(void);
	
	virtual void update(void);
	virtual void onActivate(void);
	virtual void onDeactivate(void);
	
protected:
	void queueNewMessage(const char* newMessage, ...);
	void setCharSize(float charHeight) { mCharHeight = charHeight; }
	void killAllMessages(void);
	
private:
	
	struct QueuedMessageConfig
	{
		float mMessageAscensionSpeed;
		float mMessageFadeSpeed;
		float mMessageKillTime;
	};
	
	class QueuedMessage
	{
	public:
		QueuedMessage(const QueuedMessageConfig& config, GHGeometry* geom, GHEntity& entity);
		~QueuedMessage(void);
		
		bool update(float timePassed);
		
	private:
		void onDestruction(void);
		
	private:
		GHEntity& mEntity;
		GHTransformNode mTransformNode;
		const QueuedMessageConfig& mConfig;
		float mTimeLeft;
		bool mIsActive;
	};
	std::vector<QueuedMessage*> mMessageQueue;
	std::queue<QueuedMessage*> mMessagesToQueue;
	
private:
	GHRenderGroup& mRenderGroup;
	GHGeometryRenderable* mRenderable;
	GHEntity& mEntity;
	const GHTimeVal& mTimeVal;
	const GHFontRenderer& mFontRenderer;
	GHVBFactory& mVbFactory;
	GHFont* mFont;
	float mCharHeight;
	QueuedMessageConfig mConfig;
	float mDelayBetweenMessages;
	float mLastMessageSpawnTime;
	char* mFormattingBuffer;
};

