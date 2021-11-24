// Copyright Golden Hammer Software
#pragma once

#include "GHPlatform/win32/GHWin32Include.h"
#include <dsound.h>
#include "Base/GHSoundDevice.h"

class GHWin32Window;

class GHDirectSoundDevice : public GHSoundDevice
{
public:
	GHDirectSoundDevice(GHSoundVolumeMgr& volumeMgr, GHWin32Window& window);
	~GHDirectSoundDevice(void);

	virtual void enable3DSound(bool enable);
	virtual void setListenerPosition(const GHPoint3& pos);
	virtual void setListenerOrientation(const GHPoint3& dir);
	virtual void setListenerVelocity(const GHPoint3& velocity);
	virtual GHSoundEmitter* createEmitter(GHSoundHandle& sound, const GHIdentifier& category);

	IDirectSound8* getDevice(void) { return mDirectSound; }

private:
	bool init(GHWin32Window& window);
	bool initPrimaryBuffer(void);
	void shutdown(void);

private:
	GHSoundVolumeMgr& mVolumeMgr;
	IDirectSound8* mDirectSound;
	// there's a primary buffer for the sound device.
	LPDIRECTSOUNDBUFFER mPrimaryBuffer;
};
