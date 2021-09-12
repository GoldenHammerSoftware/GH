// Copyright Golden Hammer Software
#pragma once

#include "GHXMLObjLoader.h"

class GHResourceFactory;
class GHSoundDevice;
class GHStringIdFactory;
class GHSoundEmitter;
class GHSharedSoundPool;

/*
<soundTransition activateSound="click.wav" shared="false" activateCategory="effects" looping=false activateVolume=1 deactivateSound="click2.wav" deactivateVolume=1 deactivateCategory="music"/>
*/
class GHSoundTransitionXMLLoader : public GHXMLObjLoader
{
public:  
    GHSoundTransitionXMLLoader(GHResourceFactory& resourceFactory, GHSoundDevice& device,
                               GHStringIdFactory& idFactory);
    ~GHSoundTransitionXMLLoader(void);
    
    virtual void* create(const GHXMLNode& node, GHPropertyContainer& extraData) const;
    GH_NO_POPULATE
    
private:
    GHSoundEmitter* createEmitter(const char* nameAttr, const char* volAttr,
                                  const char* catAttr, bool shared, bool& foundShared, const GHXMLNode& node) const;
    
private:
    GHSharedSoundPool* mSharedSoundPool;
    GHResourceFactory& mResourceFactory;
    GHSoundDevice& mDevice;
    GHStringIdFactory& mIdFactory;
};
