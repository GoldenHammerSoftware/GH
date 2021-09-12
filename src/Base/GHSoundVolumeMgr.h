// Copyright Golden Hammer Software
#pragma once

#include "GHString/GHIdentifier.h"
#include <list>
#include "GHSoundProfile.h"

class GHSoundEmitter;

// a class to manage applying volume to emitters.
// There is global volume applied to all,
//  and category volume in addition to global volume.
// Volume is from 0-1 and is a multiplier on raw gain.
class GHSoundVolumeMgr
{
public:
    GHSoundVolumeMgr(void);
    
    // take an unmodified sound gain and apply global volumes.
    float modifyGain(float rawGain, const GHIdentifier& category);

    // if category == 0, that's global volume.
    void setVolume(float val, const GHIdentifier& category);
    float getVolume(const GHIdentifier& category);
    
    void registerEmitter(GHSoundEmitter& emitter);
    void unregisterEmitter(GHSoundEmitter& emitter);
    
	void setEnvironmentProfile(const GHSoundProfile& envProfile);

private:
    void updateEmitterVolumes(void) const;
    
private:
    // category volumes, which are multiplied by global volume.
	// category 0 is global volume applied to all.
	GHSoundProfile mGlobalSoundProfile;
	// additional global volumes used to control relative volume between emitter categories.
	// used to set up an environment for the sound.
	GHSoundProfile mEnvironmentSoundProfile;
    // list of active emitters that we update when volume changes.
    // we do not own these.
    std::list<GHSoundEmitter*> mEmitters;
};
