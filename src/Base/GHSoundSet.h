// Copyright Golden Hammer Software
#pragma once

#include "GHSoundEmitter.h"
#include <vector>

// a special case emitter that wraps a set of sounds as if they were a single sound.
class GHSoundSet : public GHSoundEmitter
{
public:
	GHSoundSet(const GHIdentifier& category, GHSoundVolumeMgr& volumeMgr);
	~GHSoundSet(void);
	
	void addSoundEmitter(GHSoundEmitter* emitter);

	virtual void play(void);
	virtual void pause(void);
	virtual void stop(void);
	
	virtual void setPosition(const GHPoint3& pos);
	virtual void setVelocity(const GHPoint3& pos);
	virtual void setLooping(bool looping);
	virtual void setPitch(float pitch);
	virtual void seek(float t);

	virtual bool isPlaying(void) const;
	virtual bool isLooping(void) const;

	virtual void setIsPositional(bool);
	virtual bool isPositional(void) const;

private:
	virtual void applyGain(float gain);

private:
	std::vector<GHSoundEmitter*> mEmitters;
};
