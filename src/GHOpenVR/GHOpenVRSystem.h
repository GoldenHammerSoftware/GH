#pragma once
#include <openvr.h>
#include "GHUtils/GHController.h"

class GHMessageHandler; 

class GHOpenVRSystem : public GHController
{
public:
	GHOpenVRSystem(GHMessageHandler& messageHandler);
	~GHOpenVRSystem(void);

	vr::IVRSystem* getSystem(void) { return mVRSystem; };
	const vr::IVRSystem* getSystem(void) const { return mVRSystem; };
	
	uint32_t getWidth(void) const { return mWidth; };
	uint32_t getHeight(void) const { return mHeight; };

	virtual void onActivate(void) {}
	virtual void onDeactivate(void) {}
	virtual void update(void);

	bool hmdIsActive(void) const { return mHMDIsActive;  }
	void updateHmdActive(void);

private:
	void init(void);
	void shutdown(void);

private:
	GHMessageHandler& mMessageHandler;
	vr::IVRSystem* mVRSystem{ 0 };

	uint32_t mWidth{ 0 };
	uint32_t mHeight{ 0 };

	bool mHMDIsActive{ false };
};