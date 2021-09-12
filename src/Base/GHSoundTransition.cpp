// Copyright Golden Hammer Software
#include "GHSoundTransition.h"
#include "GHSoundEmitter.h"
#include "GHPlatform/GHDebugMessage.h"
#include "GHSharedSoundPool.h"

GHSoundTransition::GHSoundTransition(GHSoundEmitter* activateSound, GHSoundEmitter* deactivateSound, GHSharedSoundPool* sharedSoundPool)
: mActivateSound(activateSound)
, mDeactivateSound(deactivateSound)
, mSharedSoundPool(sharedSoundPool)
{
}

GHSoundTransition::~GHSoundTransition(void)
{
    if (mSharedSoundPool)
    {
        mSharedSoundPool->releaseSharedEmitter(mActivateSound);
        mSharedSoundPool->releaseSharedEmitter(mDeactivateSound);
    }
    else
    {
        delete mActivateSound;
        delete mDeactivateSound;
    }
}

void GHSoundTransition::activate(void)
{
    if (mActivateSound) mActivateSound->play();
}

void GHSoundTransition::deactivate(void)
{
    if (mActivateSound && mActivateSound->isLooping()) {
        mActivateSound->stop();
    }
    if (mDeactivateSound) {
        mDeactivateSound->play();
    }
}
