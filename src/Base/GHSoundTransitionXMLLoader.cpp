// Copyright Golden Hammer Software
#include "GHSoundTransitionXMLLoader.h"
#include "GHUtils/GHResourceFactory.h"
#include "GHSoundDevice.h"
#include "GHSoundEmitter.h"
#include "GHXMLNode.h"
#include "GHPlatform/GHDebugMessage.h"
#include "GHSoundTransition.h"
#include "GHSharedSoundPool.h"

GHSoundTransitionXMLLoader::GHSoundTransitionXMLLoader(GHResourceFactory& resourceFactory, GHSoundDevice& device, GHStringIdFactory& idFactory)
: mResourceFactory(resourceFactory)
, mDevice(device)
, mIdFactory(idFactory)
{
    mSharedSoundPool = new GHSharedSoundPool;
}

GHSoundTransitionXMLLoader::~GHSoundTransitionXMLLoader(void)
{
    delete mSharedSoundPool;
}

void* GHSoundTransitionXMLLoader::create(const GHXMLNode& node, GHPropertyContainer& extraData) const
{
    GHSharedSoundPool* sharedSoundPool = nullptr;
    bool shared = false;
    if (node.readAttrBool("shared", shared) && shared)
    {
        sharedSoundPool = mSharedSoundPool;
    }

    bool foundActiveShared, foundDeactiveShared;
    GHSoundEmitter* activateEmitter = createEmitter("activateSound", "activateVolume", "activateCategory", shared, foundActiveShared, node);
    GHSoundEmitter* deactivateEmitter = createEmitter("deactivateSound", "deactivateVolume", "deactivateCategory", shared, foundDeactiveShared, node);

    if (activateEmitter) 
    {
        // if it's shared then we already set the looping.
        // if we call it again it could restart a playing sound.
        if (!foundActiveShared)
        {
            bool looping = false;
            node.readAttrBool("looping", looping);
            activateEmitter->setLooping(looping);
        }
    }
    else if (!deactivateEmitter) {
        GHDebugMessage::outputString("Sound transition with no sounds!");
        return 0;
    }
    
    return new GHSoundTransition(activateEmitter, deactivateEmitter, sharedSoundPool);
}

GHSoundEmitter* GHSoundTransitionXMLLoader::createEmitter(const char* nameAttr, const char* volAttr,
                                                          const char* catAttr, bool shared, bool& foundShared, 
                                                          const GHXMLNode& node) const
{
    foundShared = false;
    const char* fileName = node.getAttribute(nameAttr);
    if (!fileName) return 0;

    GHIdentifier hashedFilename = 0;
    if (shared)
    {
        hashedFilename = mIdFactory.generateHash(fileName);
        GHSoundEmitter* ret = mSharedSoundPool->acquireSharedEmitter(hashedFilename);
        if (ret)
        {
            foundShared = true;
            return ret;
        }
    }
    
    GHSoundHandle* handle = (GHSoundHandle*)mResourceFactory.getCacheResource(fileName);
    if (handle)
    {
        GHIdentifier category = 0;
        const char* catString = node.getAttribute(catAttr);
        if (catString) {
            category = mIdFactory.generateHash(catString);
        }
        GHSoundEmitter* emitter = mDevice.createEmitter(*handle, category);
        float volume = 1.0f;
        node.readAttrFloat(volAttr, volume);
        emitter->setGain(volume);

        if (shared)
        {
            mSharedSoundPool->storeSharedEmitter(hashedFilename, emitter);
        }

        return emitter;
    }
    return 0;
}
