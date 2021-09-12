// Copyright Golden Hammer Software
#pragma once

#include "GHSoundEmitter.h"

class GHDirectSoundHandle;

class GHDirectSoundEmitter : public GHSoundEmitter
{
public:
	GHDirectSoundEmitter(GHDirectSoundHandle& handle, const GHIdentifier& category, GHSoundVolumeMgr& volumeMgr);
	virtual ~GHDirectSoundEmitter(void);

	virtual void play(void);
	virtual void pause(void);
	virtual void stop(void);

	virtual void setIsPositional(bool isPositional);
	virtual void setPosition(const GHPoint3& pos);
	virtual void setVelocity(const GHPoint3& pos);
	virtual void setLooping(bool looping);
	virtual void setPitch(float pitch);
	virtual void seek(float t);

	virtual bool isPositional(void) const;
	virtual bool isPlaying(void) const;
	virtual bool isLooping(void) const;

private:
	virtual void applyGain(float gain);

private:
	GHDirectSoundHandle& mHandle;
	bool mLooping;
};
