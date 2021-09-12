// Copyright Golden Hammer Software
#pragma once

#include "GHUtils/GHTransition.h"
#include "GHPlatform/GHRefCounted.h"

class GHSoundEmitter;
class GHSharedSoundPool;

// Plays a sound on activate and/or deactivate
// The deactivate sound can not be looping.
// If the activate sound is looping, it will be stopped on deactivate
// Takes ownership of the deletion of the emitters that are given.
class GHSoundTransition : public GHTransition
{
public:
    GHSoundTransition(GHSoundEmitter* activateSound, GHSoundEmitter* deactivateSound, GHSharedSoundPool* sharedSoundPool=nullptr);
    ~GHSoundTransition(void);
    
    virtual void activate(void);
	virtual void deactivate(void);
    
private:
    GHSoundEmitter* mActivateSound;
    GHSoundEmitter* mDeactivateSound;
    GHSharedSoundPool* mSharedSoundPool;
};
