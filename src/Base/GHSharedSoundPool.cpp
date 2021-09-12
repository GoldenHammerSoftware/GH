#include "GHSharedSoundPool.h"
#include "GHSoundEmitter.h"

GHSharedSoundPool::GHSharedSoundPool(void)
{

}

GHSharedSoundPool::~GHSharedSoundPool(void)
{
    //We could choose to clean up any remaining emitters here (maybe we should)
    // However: they should have already been cleaned up by now.
    //The resource cache will probably do a better job of catching the leaks than we will.
    // Either way, if there are any remaining references, they will crash when they try to reach back and release.
    assert(mSharedEmitters.size() == 0);
}

void GHSharedSoundPool::storeSharedEmitter(const GHIdentifier& id, GHSoundEmitter* emitter)
{
    if (!emitter) {
        return;
    }

    auto refCountedEmitter = new GHRefCountedType<GHSoundEmitter>(emitter);
    refCountedEmitter->acquire();
    mSharedEmitters[id] = refCountedEmitter;
}

GHSoundEmitter* GHSharedSoundPool::acquireSharedEmitter(const GHIdentifier& id)
{
    auto iter = mSharedEmitters.find(id);
    if (iter == mSharedEmitters.end()
        || !iter->second)
    {
        return nullptr;
    }

    iter->second->acquire();
    return iter->second->get();
}

void GHSharedSoundPool::releaseSharedEmitter(GHSoundEmitter* emitter)
{
    if (!emitter) {
        return;
    }

    auto iterEnd = mSharedEmitters.end();
    for (auto iter = mSharedEmitters.begin(); iter != iterEnd; ++iter)
    {
        auto refCountedPtr = iter->second;
        if (refCountedPtr && refCountedPtr->get() == emitter)
        {
            if (refCountedPtr->getRefCount() == 1)
            {
                mSharedEmitters.erase(iter);
            }
            refCountedPtr->release();
            return;
        }
    }
}
