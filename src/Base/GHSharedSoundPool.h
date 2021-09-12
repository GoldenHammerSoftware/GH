#pragma once

#include <map>
#include "GHString/GHIdentifier.h"
#include "GHPlatform/GHRefCounted.h"

class GHSoundEmitter;

class GHSharedSoundPool
{
public:
	GHSharedSoundPool(void);
	~GHSharedSoundPool(void);

	void releaseSharedEmitter(GHSoundEmitter* emitter);

	void storeSharedEmitter(const GHIdentifier& id, GHSoundEmitter* emitter);
	GHSoundEmitter* acquireSharedEmitter(const GHIdentifier& id);

private:
	std::map<GHIdentifier, GHRefCountedType<GHSoundEmitter>*> mSharedEmitters;
};
