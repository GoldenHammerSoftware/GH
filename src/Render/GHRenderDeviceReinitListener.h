// Copyright Golden Hammer Software
#pragma once

#include "GHUtils/GHMessageHandler.h"
#include "GHUtils/GHEventMgr.h"
#include "GHRenderProperties.h"

// Utility listener for a T that implements onDeviceReinit()
// Registers for GHRenderProperties::DEVICEREINIT
// It's a little sad that this brings in so many includes but it saves typing.
template <typename T>
class GHRenderDeviceReinitListener : public GHMessageHandler
{
public:
	GHRenderDeviceReinitListener(GHEventMgr& eventMgr, T& parent)
		: mEventMgr(eventMgr)
		, mParent(parent)
	{
		mEventMgr.registerListener(GHRenderProperties::DEVICEREINIT, *this);
	}
	~GHRenderDeviceReinitListener(void)
	{
		mEventMgr.unregisterListener(GHRenderProperties::DEVICEREINIT, *this);
	}

	void handleMessage(const GHMessage& message)
	{
		mParent.onDeviceReinit();
	}

private:
	GHEventMgr & mEventMgr;
	T& mParent;
};

// Utility listener for a T that implements handleGraphicsQualityChanged()
// Registers for GHRenderProperties::M_GRAPHICSQUALITYCHANGED
// It's a little sad that this brings in so many includes but it saves typing.
template <typename T>
class GHGraphicsQualityChangedListener : public GHMessageHandler
{
public:
	GHGraphicsQualityChangedListener(GHEventMgr& eventMgr, T& parent)
		: mEventMgr(eventMgr)
		, mParent(parent)
	{
		mEventMgr.registerListener(GHRenderProperties::M_GRAPHICSQUALITYCHANGED, *this);
	}
	~GHGraphicsQualityChangedListener(void)
	{
		mEventMgr.unregisterListener(GHRenderProperties::M_GRAPHICSQUALITYCHANGED, *this);
	}

	void handleMessage(const GHMessage& message)
	{
		mParent.handleGraphicsQualityChanged();
	}

private:
	GHEventMgr& mEventMgr;
	T& mParent;
};
